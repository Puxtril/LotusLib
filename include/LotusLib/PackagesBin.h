#pragma once

#include "BinaryReader/BufferedSlice.h"
#include "LotusLib/EENotationParser.h"
#include "LotusLib/Exceptions.h"
#include "LotusLib/Logger.h"
#include "nlohmann/json.hpp"
#include "zstd.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <mutex>
#include <string>

namespace LotusLib
{
    namespace Impl
    {
        struct RawPackagesEntity
        {
            std::string pkg;
            std::string filename;
            std::string parentType;
            int decompressedLen;
            std::vector<char> attributeData;
            bool isCompressed;
        };

        struct PackagesEntity
        {
            std::string parent;
            std::vector<char> attributeData;
            int decompressedLen;
            bool isCompressed;
        };

        struct PackagesBinState
        {
            std::map<std::string, PackagesEntity> entityMap;
            bool isInitilized;
            bool errorReading;
            int version;
            ZSTD_DDict* zstdDict;
            ZSTD_DCtx* zstdContext;
            std::mutex mutex;

            PackagesBinState();
            ~PackagesBinState();
        };
    };

    class PackagesBin
    {
        std::shared_ptr<Impl::PackagesBinState> m_state;

    public:
        PackagesBin();

        void initilize(const std::vector<uint8_t>& data);

        bool isInitilized() const;
        bool isInitSuccess() const;
        bool hasParameters(const std::string& filePath) const;
        int getVersion() const;
        std::string getParameters(const std::string& filePath) const;
        nlohmann::json getParametersJson(const std::string& filePath) const;
        const std::string& getParent(const std::string& filePath) const;
        std::map<std::string, Impl::PackagesEntity>::const_iterator begin() const;
        std::map<std::string, Impl::PackagesEntity>::const_iterator end() const;

    private:
        std::vector<Impl::RawPackagesEntity> readFile(BinaryReader::BufferedSlice& reader);
        // Versions: ?? - 44
        std::vector<Impl::RawPackagesEntity> readFile1(BinaryReader::BufferedSlice& reader);
        // Versions: 45+
        std::vector<Impl::RawPackagesEntity> readFile2(BinaryReader::BufferedSlice& reader);

        void buildEntityMap(std::vector<Impl::RawPackagesEntity>& rawEntities);

        // Sets m_zstdDict and returns
        ZSTD_DDict* createZstdDictionary(const void* dictBuffer, size_t dictSize);

        std::string readAttributes(const Impl::PackagesEntity& entity) const;

        void findValueOffsetInRange(BinaryReader::BufferedSlice& reader, uint32_t lowerBounds, uint32_t upperBound, size_t maxBytesSearch, const std::string& debugMsg) const;
    };
};
