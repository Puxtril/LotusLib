#pragma once

#include "BinaryReader/BufferedSlice.h"
#include "LotusLib/EENotationParser.h"
#include "LotusLib/Exceptions.h"
#include "LotusLib/Logger.h"
#include "LotusLib/Enums.h"
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
            bool usesZstd;
            int version;
            LotusLib::Game game;
            ZSTD_DDict* zstdDict;
            ZSTD_DCtx* zstdContext;
            std::mutex mutex;

            PackagesBinState();
            ~PackagesBinState();
        };
    };

    // Despite being named after its filename Packages.bin
    // Games before Warframe (Star Trek, Darkness II, Dark Sector) call this file Packages.cs
    //
    // THESE ARE KNOWN TO HAVE JSON DECODE ERRORS:
    // -------------------------------------------
    // /Lotus/Types/Enemies/Corpus/Spaceman/AiRifleSimaris
    // /Lotus/Sounds/Dialog/ForestEvent/Transmissions/HekProtect
    // /Lotus/Sounds/Dialog/ForestEvent/Transmissions/ObjectiveSpottedExt
    class PackagesBin
    {
        std::shared_ptr<Impl::PackagesBinState> m_state;

    public:
        PackagesBin();

        void initilize(const std::vector<uint8_t>& data, LotusLib::Game game);

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

        // Dark Sector
        std::vector<Impl::RawPackagesEntity> readFile_16(BinaryReader::BufferedSlice& reader);
        // Star Trek + Darkness II
        std::vector<Impl::RawPackagesEntity> readFile_19(BinaryReader::BufferedSlice& reader);
        // Warframe and beyond
        std::vector<Impl::RawPackagesEntity> readFile_24_28(BinaryReader::BufferedSlice& reader);
        std::vector<Impl::RawPackagesEntity> readFile_29(BinaryReader::BufferedSlice& reader);
        std::vector<Impl::RawPackagesEntity> readFile_30_31(BinaryReader::BufferedSlice& reader);
        // Starts using ZSTD
        std::vector<Impl::RawPackagesEntity> readFile_34(BinaryReader::BufferedSlice& reader);
        std::vector<Impl::RawPackagesEntity> readFile_36(BinaryReader::BufferedSlice& reader);
        std::vector<Impl::RawPackagesEntity> readFile_38_44(BinaryReader::BufferedSlice& reader);
        std::vector<Impl::RawPackagesEntity> readFile_45(BinaryReader::BufferedSlice& reader);
        std::vector<Impl::RawPackagesEntity> readFile_46(BinaryReader::BufferedSlice& reader);

        int readVersion(BinaryReader::BufferedSlice& reader);
        void buildEntityMap(std::vector<Impl::RawPackagesEntity>& rawEntities);

        // Sets m_zstdDict and returns
        ZSTD_DDict* createZstdDictionary(const void* dictBuffer, size_t dictSize);

        std::string readAttributes(const Impl::PackagesEntity& entity) const;

        // Throw LotusException if not successful
        // I guess this can be defined as a Recursive Finite State Machine
        // Resursively search for all required values, put found offsets into `offsets`
        //
        // Since we're searching for values within a range, it's entirely possible we find an incorrect value.
        // To test if it's incorrect, we must continue finding subsequent values to test if the previous one is correct.
        void findAllValueOffsets(BinaryReader::BufferedSlice& reader, std::array<size_t, 5>& offsets, int state = 0);

        // Throw LotusException if not successful
        void findValueOffsetInRange(BinaryReader::BufferedSlice& reader, uint32_t lowerBounds, uint32_t upperBound, size_t maxBytesSearch, const std::string& debugMsg) const;
    };
};
