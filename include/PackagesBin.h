#pragma once

#include <string>
#include <iostream>
#include <map>

#include "BinaryReaderBuffered.h"
#include "BinaryReaderSlice.h"
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
        ZSTD_DDict* m_zstdDict;
        ZSTD_DCtx* m_zstdContext;

        PackagesBin(const PackagesBin&) = delete;
        PackagesBin& operator=(const PackagesBin&) = delete;

    public:
        PackagesBin();
        ~PackagesBin();

        void initilize(BinaryReader::BinaryReaderBuffered& reader);
        bool hasParameters(const std::string& filePath);
        std::string getParameters(const std::string& filePath);
        const std::string& getParent(const std::string& filePath);

    private:
        std::vector<RawPackagesEntity> readFile(BinaryReader::BinaryReaderBuffered& reader);

        void buildEntityMap(std::vector<RawPackagesEntity>& rawEntities);

        // Sets m_zstdDict and returns
        ZSTD_DDict* createZstdDictionary(const void* dictBuffer, size_t dictSize);

        std::string readAttributes(const std::vector<char>& attributeData, size_t decompressedLen);
    };
};
