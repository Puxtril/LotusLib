#pragma once

#include <string>
#include <iostream>

#include "BinaryReaderBuffered.h"
#include "zstd.h"

namespace LotusLib
{
    struct PackagesEntity
    {
        std::string pkg;
        std::string filename;
        std::string parentType;
        std::string parameters;
    };

    class PackagesBin
    {
        std::vector<PackagesEntity> m_entities;

    public:
        PackagesBin() = default;

        void read(BinaryReader::BinaryReaderBuffered& reader);
    };
};