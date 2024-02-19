#pragma once

#include "LotusExceptions.h"

#include <array>
#include <string>
#include <vector>
#include <cstdint>
#include <cstring>

namespace LotusLib
{
	struct CommonHeader
	{
		std::array<unsigned char, 16> hash;
		std::vector<std::string> paths;
		std::string attributes;
		uint32_t type;

		CommonHeader() : hash(), paths(), attributes(), type(0) {}
	};

	class CommonHeaderReader
	{
	public:
		static int findHeaderLen(const std::vector<uint8_t>& file);
		static int readHeader(const std::vector<uint8_t>& file, CommonHeader& header);
		static CommonHeader readHeader(const std::vector<uint8_t>& file);
	};
}
