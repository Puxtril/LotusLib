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

	class Compression
	{
	public:
		// Warframe/Soulframe Post-Ensmallening
		static std::vector<uint8_t> decompressWarframePost(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader);
		static void decompressWarframePost(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader, uint8_t* outData);
		static std::vector<std::pair<uint32_t, uint32_t>> getWarframePostBlockLengths(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader);

		// Warframe Pre-Ensmallening
		static std::vector<uint8_t> decompressWarframePre(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader);
		static void decompressWarframePre(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader, uint8_t* outData);
		static std::vector<std::pair<uint32_t, uint32_t>> getWarframePreBlockLengths(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader);

		// Darkness II and Star Trek
		static std::vector<uint8_t> decompressEE(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader);
		static void decompressEE(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader, uint8_t* outData);
		static std::vector<std::pair<uint32_t, uint32_t>> getEEBlockLengths(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader);

		// Keystone
		static std::vector<uint8_t> decompressKeystone(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader);
		static void decompressKeystone(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader, uint8_t* outData);
		static std::vector<std::pair<uint32_t, uint32_t>> getKeystoneBlockLengths(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader);

		static void decompressDarkSector(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader, uint8_t* outData);
	private:
		static std::tuple<uint32_t, uint32_t> getWarframeBlockLens(uint8_t* data);
		static std::tuple<uint32_t, uint32_t> getEEBlockLensOodle(uint8_t* data);
		static std::tuple<uint16_t, uint16_t> getEEBlockLensLz(uint8_t* data);
		static std::tuple<uint32_t, uint32_t> getKeystoneBlockLens(uint8_t* data);
		static std::tuple<uint32_t, uint32_t> getDarkSectorBlockLens(uint8_t* data);

		static std::streampos getFileLen(std::ifstream& file);

		static void decompressOodle(uint8_t* inputData, size_t inputLen, uint8_t* outputData, size_t outputLen);
		static unsigned int decompressLz(uint8_t* inputData, uint32_t inputLen, uint8_t* outputData, uint32_t outputLen);
	};
}
