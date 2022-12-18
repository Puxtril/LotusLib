#pragma once

#include <cstdint>
#include <fstream>
#include <algorithm>

#include "Compression.h"
#include "LotusExceptions.h"
#include "DirNode.h"
#include "FileNode.h"

namespace LotusLib {
	//! \brief Decompression for Warframe-formatted data
	//!
	//! This separates the compression algorithm (Compression.h)
	//! from the compression implementation (CompressionLotus.h).
	class CompressionLotus
	{
	public:
		//! Post-Ensmallening:
		//!		8 bytes of data at the beginning of the compressed block
		//!		 will contain the compressed and decompressed block length.
		//!		The block data may be oodle-compressed or LZ compressed.
		//		Multiple blocks may need to be concatonated to build the final file.
		static char* getDataAndDecompressPost(const FileEntries::FileNode* entry, std::ifstream& cacheReader);
		//! Pre-Ensmallening:
		//!		The data is always LZ compressed, always 1 block in length.
		static char* getDataAndDecompressPre(const FileEntries::FileNode* entry, std::ifstream& cacheReader);

	private:
		static bool isOodleBlock(std::ifstream& cacheReader);
		// Reads from current m_cacheReader position
		// Sets position 8 bytes forward if contains lengths
		static std::tuple<uint32_t, uint32_t> getBlockLens(std::ifstream& cacheReader);
		static std::streampos getFileLen(std::ifstream& file);
	};
}
