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

	int commonHeaderFindLen(BinaryReader::BinaryReaderBuffered& reader);
	uint32_t commonHeaderReadFormat(BinaryReader::BinaryReaderBuffered& reader, bool seek = false);
	int commonHeaderRead(BinaryReader::BinaryReaderBuffered& reader, CommonHeader& header);
	CommonHeader commonHeaderRead(BinaryReader::BinaryReaderBuffered& reader);
}
