#pragma once

#include "LotusExceptions.h"
#include "BinaryReaderBuffered.h"

#include <array>
#include <string>
#include <vector>
#include <cstdint>
#include <cstring>

namespace LotusLib
{
	struct CommonHeader
	{
		std::array<uint8_t, 16> hash;
		std::vector<std::string> paths;
		std::string attributes;
		uint32_t type;

		CommonHeader() : hash(), paths(), attributes(), type(0) {}
	};

	int CHFindLen(BinaryReader::BinaryReaderBuffered& reader);
	int CHRead(BinaryReader::BinaryReaderBuffered& reader, CommonHeader& header);
	CommonHeader CHRead(BinaryReader::BinaryReaderBuffered& reader);
}
