#pragma once

#include <string>
#include <map>
#include <algorithm>

#include "BinaryReaderBuffered.h"
#include "BinaryReaderSlice.h"
#include "LotusPath.h"
#include "LotusLibLogger.h"
#include "LotusExceptions.h"
#include "zstd.h"

namespace LotusLib
{
    class PackagesBin
    {
        struct RawPackagesEntity
        {
            std::string pkg;
            std::string filename;
            std::string parentType;
            int decompressedLen;
            std::vector<char> attributeData;
        };

        struct PackagesEntity
        {
            std::string parent;
            std::vector<char> attributeData;
            int decompressedLen;
        };

        std::map<std::string, PackagesEntity> m_entityMap;
        bool m_isInitilized;
        bool m_errorReading;
        ZSTD_DDict* m_zstdDict;
        ZSTD_DCtx* m_zstdContext;

        const static inline std::vector<uint32_t> m_validVersions = { 40, 42, 43 };

        PackagesBin(const PackagesBin&) = delete;
        PackagesBin& operator=(const PackagesBin&) = delete;

    public:
        PackagesBin();
        ~PackagesBin();

        bool isInitilized();
        bool isInitSuccess();
        void initilize(BinaryReader::BinaryReaderBuffered& reader);
        bool hasParameters(const LotusLib::LotusPath& filePath);
        bool hasParameters(const std::string& filePath);
        std::string getParameters(const LotusLib::LotusPath& filePath);
        std::string getParameters(const std::string& filePath);
        const std::string& getParent(const LotusLib::LotusPath& filePath);
        const std::string& getParent(const std::string& filePath);

    private:
        std::vector<RawPackagesEntity> readFile(BinaryReader::BinaryReaderBuffered& reader);

        void buildEntityMap(std::vector<RawPackagesEntity>& rawEntities);

        // Sets m_zstdDict and returns
        ZSTD_DDict* createZstdDictionary(const void* dictBuffer, size_t dictSize);

        std::string readAttributes(const std::vector<char>& attributeData, size_t decompressedLen);
    };
};
