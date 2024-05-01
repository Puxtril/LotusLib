#pragma once

#include <iostream>
#include <fstream>

#include "LotusExceptions.h"
#include "Logger.h"
#include "oodle/oodle2.h"
#include "DirNode.h"
#include "FileNode.h"

namespace LotusLib {
	//! \brief Decompression algorithms
	class Compression
	{
	public:
		//! Post-Ensmallening:
		//!		8 bytes of data at the beginning of the compressed block
		//!		 will contain the compressed and decompressed block length.
		//!		The block data may be oodle-compressed or LZ compressed.
		//		Multiple blocks may need to be concatonated to build the final file.
		static std::vector<uint8_t> getDataAndDecompressPost(const FileEntries::FileNode* entry, std::ifstream& cacheReader);
		static void getDataAndDecompressPost(const FileEntries::FileNode* entry, std::ifstream& cacheReader, uint8_t* outData);

		//! Pre-Ensmallening:
		//!		The data is always LZ compressed, always 1 block in length.
		static std::vector<uint8_t> getDataAndDecompressPre(const FileEntries::FileNode* entry, std::ifstream& cacheReader);
		static void getDataAndDecompressPre(const FileEntries::FileNode* entry, std::ifstream& cacheReader, uint8_t* outData);

	private:
	
		static bool isOodleBlock(std::ifstream& cacheReader);
		// Reads from current m_cacheReader position
		// Sets position 8 bytes forward if contains lengths
		static std::tuple<uint32_t, uint32_t> getBlockLens(std::ifstream& cacheReader);
		static std::streampos getFileLen(std::ifstream& file);

		//! This will not check if the data is valid oodle or not, and will not throw any exception.
		//! 
		//! \param inputData The oodle-compressed data
		//! \param inputLen Length of the input data
		//! \param outputData Destination for the oodle-decompressed data
		//! \param outputLen Length of the output data array
		static void decompressOodle(uint8_t* inputData, size_t inputLen, uint8_t* outputData, size_t outputLen);

		//! This doesn't fully conform to LZ decompressors
		//! Will throw exceptions if bad data is encountered
		//! Code was taken from GMMan's Evolution Engine Cache Extractor
		//! https://forum.xentax.com/viewtopic.php?t=10782
		static void decompressLz(uint8_t* inputData, uint32_t inputLen, uint8_t* outputData, uint32_t outputLen);
		static void decompressLzBlock(const unsigned char* inputData, uint32_t inputLen, unsigned char* outputData, uint32_t outputLen);
		static void decompressLzBlockHelper(const unsigned char* compressedData, int32_t compressedDataLen, unsigned char* decompressedData, int32_t decompressedDataLen);
	};
}
