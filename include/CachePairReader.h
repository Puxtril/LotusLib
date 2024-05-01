#pragma once

#include "CachePair.h"
#include "LotusExceptions.h"
#include "DirectoryTree.h"
#include "LotusPath.h"
#include "Compression.h"

#include <filesystem>
#include <fstream>

namespace LotusLib
{
	//! \brief Reads files from the .cache file
	class CachePairReader : public CachePair
	{
	public:
		CachePairReader(std::filesystem::path tocPath, std::filesystem::path cachePath, bool isPostEnsmallening);

		CachePairReader(const CachePairReader&) = delete;
		CachePairReader& operator=(CachePairReader&) = delete;
		
		std::vector<uint8_t> getData(const LotusPath& internalPath) const;
		std::vector<uint8_t> getDataAndDecompress(const LotusPath& internalPath) const;
		std::vector<uint8_t> getData(const FileEntries::FileNode* entry) const;
		std::vector<uint8_t> getDataAndDecompress(const FileEntries::FileNode* entry) const;
	};
}
