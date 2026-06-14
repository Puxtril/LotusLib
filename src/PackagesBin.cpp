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

PackagesBin::PackagesBin()
    : m_state(std::make_shared<Impl::PackagesBinState>())
{
}

void
PackagesBin::initilize(const std::vector<uint8_t>& data, LotusLib::Game game)
{
    if (isInitilized())
        return;

    std::lock_guard<std::mutex> guard(m_state->mutex);
    m_state->game = game;

    BinaryReader::BufferedSlice reader(data.data(), data.size());
    m_state->zstdContext = ZSTD_createDCtx();
    ZSTD_DCtx_setParameter(m_state->zstdContext, ZSTD_d_experimentalParam1, 1);

    try
    {
        std::vector<Impl::RawPackagesEntity> rawEntities = readFile(reader);
        buildEntityMap(rawEntities);
    }
    catch (BinaryReader::LimitException& ex)
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
PackagesBin::isInitilized() const
{
    return m_state->isInitilized;
}

bool
PackagesBin::isInitSuccess() const
{
    return !m_state->errorReading;
}

bool
PackagesBin::hasParameters(const std::string& filePath) const
{
    Impl::PackagesEntity& entity = m_state->entityMap[filePath];
    return entity.attributeData.size() > 0;
}

std::string
PackagesBin::getParameters(const std::string& filePath) const
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
PackagesBin::getParametersJson(const std::string& filePath) const
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
PackagesBin::getVersion() const
{
    return m_state->version;
}

const std::string&
PackagesBin::getParent(const std::string& filePath) const
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
PackagesBin::readFile(BinaryReader::BufferedSlice& reader)
{
    reader.seek(16, std::ios::beg);
    reader.readUInt32(20, 20, "Packages.bin header size");
    m_state->version = reader.readUInt32(40, 100, "Packages.bin Version");
    reader.readUInt32(1, 1, "Packages.bin flags");

    if (m_state->version < 45)
        return readFile1(reader);
    else if (m_state->version == 45)
        return readFile2(reader);
    else
        return readFile3(reader);
}

std::vector<Impl::RawPackagesEntity>
PackagesBin::readFile1(BinaryReader::BufferedSlice& reader)
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
    BinaryReader::BufferedSlice comFlagsBuf = reader.getSlice(comFlagsBufLen);

    uint32_t comSizeBufferLen = reader.readUInt32();
    BinaryReader::BufferedSlice comSizeBuffer = reader.getSlice(comSizeBufferLen);

    uint32_t comZBufferLen = reader.readUInt32();
    BinaryReader::BufferedSlice comZBuffer = reader.getSlice(comZBufferLen);

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
            BinaryReader::BufferedSlice frameData = comZBuffer.getSlice(size);

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
PackagesBin::readFile2(BinaryReader::BufferedSlice& reader)
{
    std::array<size_t, 5> valueOffsets;
    findAllValueOffsets(reader, valueOffsets);

    reader.seek(valueOffsets[0], std::ios::beg);
    uint32_t referenceCount = reader.readUInt32(0, 1000, "Packages.bin Reference count");
    for (size_t i = 0; i < referenceCount; i++)
    {
        uint32_t refNameLen = reader.readUInt32(0, 2000, "Packages.bin Reference name");
        reader.seek(refNameLen + 2, std::ios::cur);
    }

    reader.seek(valueOffsets[1], std::ios::beg);
    uint32_t comFlagsBufLen = reader.readUInt32();
    BinaryReader::BufferedSlice comFlagsBuf = reader.getSlice(comFlagsBufLen);

    reader.seek(valueOffsets[2], std::ios::beg);
    uint32_t comSizeBufferLen = reader.readUInt32();
    BinaryReader::BufferedSlice comSizeBuffer = reader.getSlice(comSizeBufferLen);

    reader.seek(valueOffsets[3], std::ios::beg);
    uint32_t comZBufferLen = reader.readUInt32();
    BinaryReader::BufferedSlice comZBuffer = reader.getSlice(comZBufferLen);

    // Begin reading Zstd data
    uint32_t dictSize = comSizeBuffer.readUInt32(80000, 1300000, "ZDictSize");
    
    createZstdDictionary(comZBuffer.getPtr(), dictSize);

    comZBuffer.seek(dictSize, std::ios::cur);

    uint32_t entityCount = reader.readUInt32(400000, 600000, "Entity count");
    std::vector<Impl::RawPackagesEntity> entities(entityCount);

    unsigned char flagBufferCurrentByte = comFlagsBuf.readUInt8();
    size_t flagBufferCurrentBit = 0;

    reader.seek(valueOffsets[4], std::ios::beg);

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
            BinaryReader::BufferedSlice frameData = comZBuffer.getSlice(size);

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
PackagesBin::readFile3(BinaryReader::BufferedSlice& reader)
{
    std::array<size_t, 5> valueOffsets;
    findAllValueOffsets(reader, valueOffsets);

    reader.seek(valueOffsets[0], std::ios::beg);
    uint32_t referenceCount = reader.readUInt32(0, 1000, "Packages.bin Reference count");
    for (size_t i = 0; i < referenceCount; i++)
    {
        uint32_t refNameLen = reader.readUInt32(0, 2000, "Packages.bin Reference name");
        reader.seek(refNameLen + 2, std::ios::cur);
    }

    reader.seek(valueOffsets[1], std::ios::beg);
    uint32_t comFlagsBufLen = reader.readUInt32();
    BinaryReader::BufferedSlice comFlagsBuf = reader.getSlice(comFlagsBufLen);

    reader.seek(valueOffsets[2], std::ios::beg);
    uint32_t comSizeBufferLen = reader.readUInt32();
    BinaryReader::BufferedSlice comSizeBuffer = reader.getSlice(comSizeBufferLen);

    reader.seek(valueOffsets[3], std::ios::beg);
    uint32_t comZBufferLen = reader.readUInt32();
    BinaryReader::BufferedSlice comZBuffer = reader.getSlice(comZBufferLen);

    // Begin reading Zstd data
    uint32_t dictSize = comSizeBuffer.readUInt32();
    
    createZstdDictionary(comZBuffer.getPtr(), dictSize);

    comZBuffer.seek(dictSize, std::ios::cur);

    uint32_t entityCount = reader.readUInt32();
    // I dunno, probably missing something here.
    if (m_state->game == LotusLib::Game::SOULFRAME)
        entityCount--;
    std::vector<Impl::RawPackagesEntity> entities(entityCount);

    unsigned char flagBufferCurrentByte = comFlagsBuf.readUInt8();
    size_t flagBufferCurrentBit = 0;

    reader.seek(valueOffsets[4], std::ios::beg);

    uint32_t iEntity = 0;
    while (iEntity < entityCount)
    {
        uint32_t pkgNameLen = reader.readUInt32(1, 200, "PkgNameLen");
        std::string curPkgName = reader.readAsciiString(pkgNameLen);
        
        reader.seek(1, std::ios::cur);
        uint32_t childrenCount = reader.readUInt32();
        
        for (uint32_t x = 0; x < childrenCount; x++)
        {
            Impl::RawPackagesEntity& curEntity = entities[iEntity];
            curEntity.pkg = curPkgName;

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
                BinaryReader::BufferedSlice frameData = comZBuffer.getSlice(size);

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

            iEntity++;
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
PackagesBin::readAttributes(const Impl::PackagesEntity& entity) const
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
PackagesBin::findAllValueOffsets(BinaryReader::BufferedSlice& reader, std::array<size_t, 5>& offsets, int state)
{
    size_t start = reader.tell();
    size_t cursor = start;

    // Separate ranges by game.
    // This will only search the first 5 values recursively.
    // The next 2 values (index 5 and 6) are used for extra validation,
    //   but it's just easier to put those values in this variable, because they also vary by game.
    const std::vector<std::tuple<uint32_t, uint32_t, std::string>> searchValueRangesWarframe = {
        {45, 60, "ReferenceCount"},
        {75000, 90000, "ComFlagsBufLen"},
        {210000, 240000, "ComSizeBufferLen"},
        {18000000, 21000000, "ComZBufferLen"},
        {9, 16, "PkgNameLen"},
        {80000, 1300000, "DictSize"},
        {400000, 600000, "EntityCount"},
    };
    const std::vector<std::tuple<uint32_t, uint32_t, std::string>> searchValueRangesSoulframe = {
        {35, 50, "ReferenceCount"},
        {10000, 16000, "ComFlagsBufLen"},
        {28000, 32000, "ComSizeBufferLen"},
        {3000000, 5000000, "ComZBufferLen"},
        {9, 16, "PkgNameLen"},
        {80000, 1300000, "DictSize"},
        {60000, 100000, "EntityCount"},
    };

    // Only 2 games contain Packages.bin requiring this
    const std::vector<std::tuple<uint32_t, uint32_t, std::string>> searchValueRanges = m_state->game == LotusLib::Game::WARFRAME ? searchValueRangesWarframe : searchValueRangesSoulframe;

    while (true)
    {
        if (cursor - start > 1000)
            throw LotusException("Could not find value in state");
        
        // Search for the value
        const auto& searchValue = searchValueRanges[state];
        findValueOffsetInRange(reader, std::get<0>(searchValue), std::get<1>(searchValue), 1000, std::get<2>(searchValue));
        cursor = reader.tell();
        uint32_t value = reader.readUInt32();
        
        // Process value
        // Some states skip the slice, some verify data.
        // We may continue in the loop if we determine this value invalid.
        switch(state)
        {
            case 0:
            {
                try
                {
                    for (size_t i = 0; i < value; i++)
                    {
                        uint32_t refNameLen = reader.readUInt32(0, 1000, "ReferenceNameLen");
                        reader.seek(refNameLen + 2, std::ios::cur);
                    }
                }
                catch (BinaryReader::LimitException&)
                {
                    cursor += 4;
                    reader.seek(cursor, std::ios::beg);
                    continue;
                }
                break;
            }
            case 1:
            {
                reader.seek(value, std::ios::cur);
                break;
            }
            case 2:
            {
                uint32_t dictSize = reader.readUInt32();
                if (!(dictSize > std::get<0>(searchValueRanges[5]) && dictSize < std::get<1>(searchValueRanges[5])))
                {
                    cursor += 4;
                    continue;
                }
                reader.seek(value - 4, std::ios::cur);
                break;
            }
            case 3:
            {
                reader.seek(value, std::ios::cur);

                uint32_t entityCount = reader.readUInt32();
                if (!(entityCount > std::get<0>(searchValueRanges[6]) && entityCount < std::get<1>(searchValueRanges[6])))
                {
                    cursor += 4;
                    reader.seek(cursor, std::ios::beg);
                    continue;
                }
            }
        }
        
        // Still in the loop, assuming the value is valid
        // Set offset in array, move to next state
        offsets[state] = cursor;

        // Found the last value, can return
        if (state == 4)
            return;

        try
        {
            findAllValueOffsets(reader, offsets, state + 1);
            return;
        }
        // Next state wasn't valid
        // Advance cursor and try again
        catch (LotusException&)
        {
            cursor += 4;
            reader.seek(cursor, std::ios::beg);
            continue;
        }
    }
}

void
PackagesBin::findValueOffsetInRange(BinaryReader::BufferedSlice& reader, uint32_t lowerBounds, uint32_t upperBound, size_t maxBytesSearch, const std::string& debugMsg) const
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
