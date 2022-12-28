#pragma once

#include "CachePair.h"
#include "LotusExceptions.h"
#include "DirectoryTree.h"
#include "LotusPath.h"
#include "CompressionLotus.h"

#include <filesystem>
#include <fstream>
#include <memory>

namespace LotusLib
{
	//! \brief Reads files from the .cache file
	class CachePairReader : public CachePair
	{
	public:
		CachePairReader(std::filesystem::path tocPath, std::filesystem::path cachePath, bool isPostEnsmallening);

		CachePairReader(const CachePairReader&) = delete;
		CachePairReader& operator=(CachePairReader&) = delete;
		
		//! This will not check if enough space has been allocated
		int getData(const LotusPath& internalPath, char* data) const;
		//! This will not check if enough space has been allocated
		int getDataAndDecompress(const LotusPath& internalPath, char* outData) const;
		std::unique_ptr<char[]> getData(const FileEntries::FileNode* entry) const;
		std::unique_ptr<char[]> getDataAndDecompress(const FileEntries::FileNode* entry) const;
	};
}
