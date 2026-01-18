#pragma once

#include "LotusLib/Exceptions.h"
#include "LotusLib/TOCTree.h"
#include "lzf.h"
#include "oodle/oodle2.h"

#include <fstream>

namespace LotusLib::Impl {

	struct CompressionScratch
	{
		std::vector<uint8_t> buf;
		CompressionScratch(): buf(0x40000) {}
	};

	//! \brief Decompression algorithms
	class Compression
	{
	public:
		// Warframe/Soulframe Post-Ensmallening
		static std::vector<uint8_t> decompressWarframePost(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader);
		static void decompressWarframePost(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader, uint8_t* outData);

		// Warframe Pre-Ensmallening
		static std::vector<uint8_t> decompressWarframePre(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader);
		static void decompressWarframePre(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader, uint8_t* outData);

		// Darkness II and Star Trek
		static std::vector<uint8_t> decompressEE(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader);
		static void decompressEE(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader, uint8_t* outData);

	private:
		static std::tuple<uint32_t, uint32_t> getWarframeBlockLens(uint8_t* data);
		static std::tuple<uint32_t, uint32_t> getEEBlockLensOodle(uint8_t* data);
		static std::tuple<uint16_t, uint16_t> getEEBlockLensLz(uint8_t* data);

		static std::streampos getFileLen(std::ifstream& file);

		static void decompressOodle(uint8_t* inputData, size_t inputLen, uint8_t* outputData, size_t outputLen);
		static void decompressLz(uint8_t* inputData, uint32_t inputLen, uint8_t* outputData, uint32_t outputLen);
	};
}
