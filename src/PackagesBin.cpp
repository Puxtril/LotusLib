#include "LotusLib/PackagesBin.h"

using namespace LotusLib;

Impl::PackagesBinState::PackagesBinState()
    : isInitilized(false), errorReading(false)
{
}

Impl::PackagesBinState::~PackagesBinState()
{
    if (isInitilized && !errorReading)
    {
        ZSTD_freeDCtx(zstdContext);
        ZSTD_freeDDict(zstdDict);
    }
}

bool
PackagesBin::isInitilized()
{
    return m_state->isInitilized;
}

bool
PackagesBin::isInitSuccess()
{
    return !m_state->errorReading;
}

void
PackagesBin::initilize(BinaryReader::BinaryReaderBuffered& reader)
{
    if (isInitilized())
        return;

    std::lock_guard<std::mutex> guard(m_state->mutex);

    m_state->zstdContext = ZSTD_createDCtx();
    ZSTD_DCtx_setParameter(m_state->zstdContext, ZSTD_d_experimentalParam1, 1);

    try
    {
        std::vector<Impl::RawPackagesEntity> rawEntities = readFile(reader);
        buildEntityMap(rawEntities);
    }
    catch (LimitException& ex)
    {
        logError("Packages.bin read error: " + std::string(ex.what()));
        m_state->errorReading = true;
    }
    catch (LotusException& ex)
    {
        logError("Packages.bin read error: " + std::string(ex.what()));
        m_state->errorReading = true;
    }

    m_state->isInitilized = true;
}

bool
PackagesBin::hasParameters(const std::string& filePath)
{
    Impl::PackagesEntity& entity = m_state->entityMap[filePath];
    return entity.attributeData.size() > 0;
}

std::string
PackagesBin::getParameters(const std::string& filePath)
{
    try
    {
        Impl::PackagesEntity& entity = m_state->entityMap.at(filePath);
        return readAttributes(entity);
    }
    catch (std::out_of_range&)
    {
        return std::string();
    }
}

nlohmann::json
PackagesBin::getParametersJson(const std::string& filePath)
{
    try
    {
        Impl::PackagesEntity& entity = m_state->entityMap.at(filePath);
        std::string attrs = readAttributes(entity);
        return EENotationParser::parse(attrs.c_str(), attrs.size());
    }
    catch (std::out_of_range&)
    {
        return std::string();
    }
}

int
PackagesBin::getVersion()
{
    return m_state->version;
}

const std::string&
PackagesBin::getParent(const std::string& filePath)
{
    try
    {
        Impl::PackagesEntity& entity = m_state->entityMap.at(filePath);
        return entity.parent;
    }
    catch (std::out_of_range&)
    {
        static const std::string empty;
        return empty;
    }
}

std::map<std::string, Impl::PackagesEntity>::const_iterator
PackagesBin::begin() const
{
    return m_state->entityMap.begin();
}

std::map<std::string, Impl::PackagesEntity>::const_iterator
PackagesBin::end() const
{
    return m_state->entityMap.end();
}

std::vector<Impl::RawPackagesEntity>
PackagesBin::readFile(BinaryReader::BinaryReaderBuffered& reader)
{
    reader.seek(16, std::ios::beg);
    reader.readUInt32(20, 20, "Packages.bin header size");
    m_state->version = reader.readUInt32(40, 100, "Packages.bin Version");
    reader.readUInt32(1, 1, "Packages.bin flags");

    if (m_state->version < 45)
        return readFile1(reader);
    else
        return readFile2(reader);
}

std::vector<Impl::RawPackagesEntity>
PackagesBin::readFile1(BinaryReader::BinaryReaderBuffered& reader)
{
    reader.seek(16, std::ios::beg);
    reader.readUInt32(20, 20, "Packages.bin header size");
    m_state->version = reader.readUInt32(30, 100, "Packages.bin Version");
    reader.readUInt32(1, 1, "Packages.bin flags");

    // ???
    reader.seek(4, std::ios::cur);

    std::vector<std::string> references;
    uint32_t referenceCount = reader.readUInt32(0, 1000, "Packages.bin Reference count");
    references.resize(referenceCount);

    for (size_t i = 0; i < referenceCount; i++)
    {
        uint32_t refNameLen = reader.readUInt32(0, 2000, "Packages.bin Reference name");
        references[i] = reader.readAsciiString(refNameLen);
        // Unknown
        reader.seek(2, std::ios::cur);
    }

    reader.readUInt32(0, 0, "Packages.bin Package count");

    uint32_t comFlagsBufLen = reader.readUInt32();
    BinaryReader::BinaryReaderSlice comFlagsBuf = reader.slice(comFlagsBufLen);

    uint32_t comSizeBufferLen = reader.readUInt32();
    BinaryReader::BinaryReaderSlice comSizeBuffer = reader.slice(comSizeBufferLen);

    uint32_t comZBufferLen = reader.readUInt32();
    BinaryReader::BinaryReaderSlice comZBuffer = reader.slice(comZBufferLen);

    uint32_t dictSize = comSizeBuffer.readUInt32();
    
    createZstdDictionary(comZBuffer.getPtr(), dictSize);

    comZBuffer.seek(dictSize, std::ios::cur);

    uint32_t entityCount = reader.readUInt32();
    std::vector<Impl::RawPackagesEntity> entities(entityCount);

    unsigned char flagBufferCurrentByte = comFlagsBuf.readUInt8();
    size_t flagBufferCurrentBit = 0;

    for (uint32_t i = 0; i < entityCount; i++)
    {
        Impl::RawPackagesEntity& curEntity = entities[i];

        uint32_t pkgNameLen = reader.readUInt32();
        curEntity.pkg = reader.readAsciiString(pkgNameLen);

        uint32_t fileNameLen = reader.readUInt32();
        curEntity.filename = reader.readAsciiString(fileNameLen);

        // short + byte
        reader.seek(3, std::ios::cur);

        uint32_t parentTypeLen = reader.readUInt32();
        curEntity.parentType = reader.readAsciiString(parentTypeLen);

        // Entity
        unsigned char hasText = flagBufferCurrentByte >> flagBufferCurrentBit++ & 1;
        if (flagBufferCurrentBit > 7)
        {
            flagBufferCurrentByte = comFlagsBuf.readUInt8();
            flagBufferCurrentBit -= 8;
        }

        std::string textParameters;

        if (hasText > 0)
        {
            uint64_t size = comSizeBuffer.readULEB(32);
            BinaryReader::BinaryReaderSlice frameData = comZBuffer.slice(size);

            curEntity.isCompressed = (flagBufferCurrentByte >> flagBufferCurrentBit++ & 1) > 0;
            if (flagBufferCurrentBit > 7)
            {
                flagBufferCurrentByte = comFlagsBuf.readUInt8();
                flagBufferCurrentBit -= 8;
            }

            if (curEntity.isCompressed)
                curEntity.decompressedLen = frameData.readULEB(32);
            else
                curEntity.decompressedLen = size;

            curEntity.attributeData.resize(size);
            memcpy(curEntity.attributeData.data(), frameData.getPtr() + frameData.tell(), size);
            frameData.seek(size, std::ios::cur);
        }
    }

    return entities;
}

std::vector<Impl::RawPackagesEntity>
PackagesBin::readFile2(BinaryReader::BinaryReaderBuffered& reader)
{
    findValueOffsetInRange(reader, 45, 60, 500, "ReferenceCount");
    uint32_t referenceCount = reader.readUInt32(0, 1000, "Packages.bin Reference count");
    for (size_t i = 0; i < referenceCount; i++)
    {
        uint32_t refNameLen = reader.readUInt32(0, 2000, "Packages.bin Reference name");
        reader.seek(refNameLen + 2, std::ios::cur);
    }

    findValueOffsetInRange(reader, 75000, 90000, 1000, "ComFlagsBufLen");
    uint32_t comFlagsBufLen = reader.readUInt32();
    BinaryReader::BinaryReaderSlice comFlagsBuf = reader.slice(comFlagsBufLen);

    findValueOffsetInRange(reader, 210000, 240000, 1000, "ComSizeBufferLen");
    uint32_t comSizeBufferLen = reader.readUInt32();
    BinaryReader::BinaryReaderSlice comSizeBuffer = reader.slice(comSizeBufferLen);

    findValueOffsetInRange(reader, 18000000, 21000000, 1000, "ComZBufferLen");
    uint32_t comZBufferLen = reader.readUInt32();
    BinaryReader::BinaryReaderSlice comZBuffer = reader.slice(comZBufferLen);

    // Begin reading Zstd data
    uint32_t dictSize = comSizeBuffer.readUInt32();
    
    createZstdDictionary(comZBuffer.getPtr(), dictSize);

    comZBuffer.seek(dictSize, std::ios::cur);

    uint32_t entityCount = reader.readUInt32();
    std::vector<Impl::RawPackagesEntity> entities(entityCount);

    unsigned char flagBufferCurrentByte = comFlagsBuf.readUInt8();
    size_t flagBufferCurrentBit = 0;

    findValueOffsetInRange(reader, 5, 30, 1000, "PkgNameLen");

    for (uint32_t i = 0; i < entityCount; i++)
    {
        Impl::RawPackagesEntity& curEntity = entities[i];

        uint32_t pkgNameLen = reader.readUInt32(1, 200, "PkgNameLen");
        curEntity.pkg = reader.readAsciiString(pkgNameLen);

        uint32_t fileNameLen = reader.readUInt32(1, 200, "FileNameLen");
        curEntity.filename = reader.readAsciiString(fileNameLen);

        // short + byte
        reader.seek(3, std::ios::cur);

        uint32_t parentTypeLen = reader.readUInt32();
        curEntity.parentType = reader.readAsciiString(parentTypeLen);

        // Entity
        unsigned char hasText = flagBufferCurrentByte >> flagBufferCurrentBit++ & 1;
        if (flagBufferCurrentBit > 7)
        {
            flagBufferCurrentByte = comFlagsBuf.readUInt8();
            flagBufferCurrentBit -= 8;
        }

        std::string textParameters;

        if (hasText > 0)
        {
            uint64_t size = comSizeBuffer.readULEB(32);
            BinaryReader::BinaryReaderSlice frameData = comZBuffer.slice(size);

            curEntity.isCompressed = (flagBufferCurrentByte >> flagBufferCurrentBit++ & 1) > 0;
            if (flagBufferCurrentBit > 7)
            {
                flagBufferCurrentByte = comFlagsBuf.readUInt8();
                flagBufferCurrentBit -= 8;
            }

            if (curEntity.isCompressed)
                curEntity.decompressedLen = frameData.readULEB(32);
            else
                curEntity.decompressedLen = size;

            curEntity.attributeData.resize(size);
            memcpy(curEntity.attributeData.data(), frameData.getPtr() + frameData.tell(), size);
            frameData.seek(size, std::ios::cur);
        }
    }

    return entities;
}

void
PackagesBin::buildEntityMap(std::vector<Impl::RawPackagesEntity>& rawEntities)
{
    for (size_t i = 0; i < rawEntities.size(); i++)
    {
        Impl::RawPackagesEntity& curRawEntity = rawEntities[i];

        std::string fullEntityPath = curRawEntity.pkg + curRawEntity.filename;
        Impl::PackagesEntity& curEntity = m_state->entityMap[fullEntityPath];

        curEntity.isCompressed = curRawEntity.isCompressed;
        curEntity.decompressedLen = curRawEntity.decompressedLen;
        curEntity.attributeData = std::move(curRawEntity.attributeData);

        if (curRawEntity.parentType[0] == '/')
            curEntity.parent = curRawEntity.parentType;
        else
            curEntity.parent = curRawEntity.pkg + curRawEntity.parentType;
    }
}

ZSTD_DDict*
PackagesBin::createZstdDictionary(const void* dictBuffer, size_t dictSize)
{
    m_state->zstdDict = ZSTD_createDDict(dictBuffer, dictSize);

    if (m_state->zstdDict == nullptr)
        throw LotusException("ZSTD create dictionary failed");

    return m_state->zstdDict;
}

std::string
PackagesBin::readAttributes(const Impl::PackagesEntity& entity)
{
    if (!entity.isCompressed)
    {
        return std::string(entity.attributeData.data(), entity.decompressedLen);
    }
    else
    {
        std::vector<uint8_t> decompressedData(entity.decompressedLen);

        std::lock_guard<std::mutex> guard(m_state->mutex);

        ZSTD_decompress_usingDDict(
            m_state->zstdContext,
            decompressedData.data(),
            entity.decompressedLen,
            entity.attributeData.data(),
            entity.attributeData.size(),
            m_state->zstdDict
        );

        return std::string((char*)decompressedData.data(), entity.decompressedLen);
    }
}

void
PackagesBin::findValueOffsetInRange(BinaryReader::BinaryReaderBuffered& reader, uint32_t lowerBounds, uint32_t upperBound, size_t maxBytesSearch, const std::string& debugMsg)
{
    size_t start = reader.tell();
    maxBytesSearch = std::min(reader.getLength() - start, maxBytesSearch);

    while (reader.tell() - start < (maxBytesSearch - 4))
    {
        uint32_t test = reader.readUInt32();
        if (test > lowerBounds && test < upperBound)
        {
            reader.seek(-4, std::ios::cur);
            return;
        }
        reader.seek(-3, std::ios::cur);
    }

    throw LotusException("Unable to find value in Packages.bin: " + debugMsg);
}
