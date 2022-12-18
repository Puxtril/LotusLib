#pragma once

#include "CachePair.h"
#include "LotusExceptions.h"
#include "DirectoryTree.h"
#include "LotusPath.h"
#include "CompressionLotus.h"

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

		int getData(const LotusPath& internalPath, char* data) const;
		char* getData(const FileEntries::FileNode* entry) const;
		int getDataAndDecompress(const LotusPath& internalPath, char* outData) const;
		char* getDataAndDecompress(const FileEntries::FileNode* entry) const;
	};
}
