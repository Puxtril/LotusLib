#pragma once

#include <iostream>
#include <fstream>

#include "LotusExceptions.h"
#include "LotusLibLogger.h"
#include "oodle/oodle2.h"
#include "DirNode.h"
#include "FileNode.h"
#include "lzf.h"

namespace LotusLib {
	//! \brief Decompression algorithms
	class Compression
	{
	public:
		// Post-Ensmallening:
		//		8 bytes of data at the beginning of the compressed block
		//		 will contain the compressed and decompressed block length.
		//		The block data may be oodle-compressed or LZ compressed.
		//		Multiple blocks may need to be concatonated to build the final file.
		static std::vector<uint8_t> decompressWarframePost(const FileEntries::FileNode* entry, std::ifstream& cacheReader);
		static void decompressWarframePost(const FileEntries::FileNode* entry, std::ifstream& cacheReader, uint8_t* outData);

		// Pre-Ensmallening:
		//		The data is always LZ compressed, always 1 block in length.
		static std::vector<uint8_t> decompressWarframePre(const FileEntries::FileNode* entry, std::ifstream& cacheReader);
		static void decompressWarframePre(const FileEntries::FileNode* entry, std::ifstream& cacheReader, uint8_t* outData);

		// Used for Darkness II and Star Trek
		static std::vector<uint8_t> decompressEE(const FileEntries::FileNode* entry, std::ifstream& cacheReader);
		static void decompressEE(const FileEntries::FileNode* entry, std::ifstream& cacheReader, uint8_t* outData);

	private:
		static uint8_t m_compressedBuffer[0x40000];

		static bool isWarframeOodleBlock(std::ifstream& cacheReader);
		static bool isEEOodleBlock(std::ifstream& cacheReader);

		// Reads from current m_cacheReader position
		// Sets position 8 bytes forward if contains lengths
		static std::tuple<uint32_t, uint32_t> getWarframeBlockLens(std::ifstream& cacheReader);
		static std::tuple<uint16_t, uint16_t> getEEBlockLensLz(std::ifstream& cacheReader);
		static std::tuple<uint32_t, uint32_t> getEEBlockLensOodle(std::ifstream& cacheReader);

		static std::streampos getFileLen(std::ifstream& file);

		// This will not check if the data is valid oodle or not, and will not throw any exception.
		// 
		// \param inputData The oodle-compressed data
		// \param inputLen Length of the input data
		// \param outputData Destination for the oodle-decompressed data
		// \param outputLen Length of the output data array
		static void decompressOodle(uint8_t* inputData, size_t inputLen, uint8_t* outputData, size_t outputLen);

		static void decompressLz(uint8_t* inputData, uint32_t inputLen, uint8_t* outputData, uint32_t outputLen);
	};
}
