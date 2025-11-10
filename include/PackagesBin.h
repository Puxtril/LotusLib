#pragma once

#include <string>
#include <map>
#include <algorithm>
#include <cmath>

#include "BinaryReaderBuffered.h"
#include "BinaryReaderSlice.h"
#include "LotusPath.h"
#include "LotusLibLogger.h"
#include "LotusExceptions.h"
#include "nlohmann/json.hpp"
#include "LotusNotationParser.h"
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
        int m_version;
        ZSTD_DDict* m_zstdDict;
        ZSTD_DCtx* m_zstdContext;

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
        int getVersion();
        std::string getParameters(const LotusLib::LotusPath& filePath);
        std::string getParameters(const std::string& filePath);
        nlohmann::json getParametersJson(const LotusLib::LotusPath& filePath);
        nlohmann::json getParametersJson(const std::string& filePath);
        const std::string& getParent(const LotusLib::LotusPath& filePath);
        const std::string& getParent(const std::string& filePath);
        std::map<std::string, PackagesEntity>::const_iterator begin() const;
        std::map<std::string, PackagesEntity>::const_iterator end() const;

    private:
        std::vector<RawPackagesEntity> readFile(BinaryReader::BinaryReaderBuffered& reader);
        // Versions: ?? - 44
        std::vector<RawPackagesEntity> readFile1(BinaryReader::BinaryReaderBuffered& reader);
        // Versions: 45+
        std::vector<RawPackagesEntity> readFile2(BinaryReader::BinaryReaderBuffered& reader);

        void buildEntityMap(std::vector<RawPackagesEntity>& rawEntities);

        // Sets m_zstdDict and returns
        ZSTD_DDict* createZstdDictionary(const void* dictBuffer, size_t dictSize);

        std::string readAttributes(const std::vector<char>& attributeData, size_t decompressedLen);

        void findValueOffsetInRange(BinaryReader::BinaryReaderBuffered& reader, uint32_t lowerBounds, uint32_t upperBound, size_t maxBytesSearch, const std::string& debugMsg);
    };
};
