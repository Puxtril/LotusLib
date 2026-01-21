#pragma once

#include "BinaryReader/BinaryReader.h"
#include "LotusLib/Enums.h"
#include "LotusLib/Exceptions.h"
#include "LotusLib/Utils.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

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

	int commonHeaderFindLen(BinaryReader::BinaryReader& reader, LotusLib::Game game);
	uint32_t commonHeaderReadFormat(BinaryReader::BinaryReader& reader, LotusLib::Game game, bool seek = false);
	int commonHeaderRead(BinaryReader::BinaryReader& reader, CommonHeader& header, LotusLib::Game game);
	CommonHeader commonHeaderRead(BinaryReader::BinaryReader& reader, LotusLib::Game game);
}
