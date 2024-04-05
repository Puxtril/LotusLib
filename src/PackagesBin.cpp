#include "PackagesBin.h"

using namespace LotusLib;

void
PackagesBin::read(BinaryReader::BinaryReaderBuffered& reader)
{
    reader.seek(16, std::ios::beg);
    uint32_t headerSize = reader.readUInt32(20, 20, "Packages.bin header size");
    uint32_t version = reader.readUInt32(40, 40, "Packages.bin version");
    uint32_t flags = reader.readUInt32(1, 1, "Packages.bin flags");

    // ???
    reader.readUInt32();

    std::vector<std::string> references;
    uint32_t referenceCount = reader.readUInt32();
    references.resize(referenceCount);

    for (size_t i = 0; i < referenceCount; i++)
    {
        uint32_t refNameLen = reader.readUInt32();
        references[i] = reader.readAsciiString(refNameLen);
        // Unknown
        reader.readUInt16();
    }

    uint32_t packageCount = reader.readUInt32(0, 0, "Package count");

    uint32_t comFlagsBufLen = reader.readUInt32();
    BinaryReader::BinaryReaderBuffered comFlagsBuf = reader.slice(comFlagsBufLen);

    uint32_t comSizeBufferLen = reader.readUInt32();
    BinaryReader::BinaryReaderBuffered comSizeBuffer = reader.slice(comSizeBufferLen);

    uint32_t comZBufferLen = reader.readUInt32();
    BinaryReader::BinaryReaderBuffered comZBuffer = reader.slice(comZBufferLen);

    uint32_t dictSize = comSizeBuffer.readUInt32();
    
    ZSTD_DDict* zstdDict = ZSTD_createDDict(comZBuffer.getPtr(), dictSize);
    if (zstdDict == nullptr)
        throw std::runtime_error("ZSTD create dictionary failed");
    comZBuffer.seek(dictSize, std::ios::cur);

    ZSTD_DCtx* zContext = ZSTD_createDCtx();
    ZSTD_DCtx_setParameter(zContext, ZSTD_d_experimentalParam1, 1);

    uint32_t entityCount = reader.readUInt32();
    m_entities.resize(entityCount);

    unsigned char flagBufferCurrentByte = comFlagsBuf.readUInt8();
    size_t flagBufferCurrentBit = 0;

    for (uint32_t i = 0; i < entityCount; i++)
    {
        PackagesEntity& curEntity = m_entities[i];

        uint32_t pkgNameLen = reader.readUInt32();
        curEntity.pkg = reader.readAsciiString(pkgNameLen);

        uint32_t fileNameLen = reader.readUInt32();
        curEntity.filename = reader.readAsciiString(fileNameLen);

        uint16_t unk = reader.readUInt16();
        uint8_t unk2 = reader.readUInt8();

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
            BinaryReader::BinaryReaderBuffered frameData = comZBuffer.slice(size);

            unsigned char isCompressed = flagBufferCurrentByte >> flagBufferCurrentBit++ & 1;
            if (flagBufferCurrentBit > 8)
            {
                flagBufferCurrentByte = comFlagsBuf.readUInt8();
                flagBufferCurrentBit -= 8;
            }
            
            if (isCompressed == 0)
            {
                curEntity.parameters = frameData.readAsciiString(size);
                continue;
            }

            uint64_t decompressedSize = frameData.readULEB(32);
            std::vector<char> decompressed(decompressedSize);

            size_t ret = ZSTD_decompress_usingDDict(
                zContext,
                decompressed.data(),
                decompressedSize,
                frameData.getPtr() + frameData.tell(),
                size,
                zstdDict
            );

            curEntity.parameters = std::string(decompressed.data(), decompressedSize);
        }
    }

    ZSTD_freeDCtx(zContext);
}