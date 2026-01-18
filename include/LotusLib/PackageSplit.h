#pragma once

#include "BinaryReaderSlice.h"
#include "LotusLib/CommonHeader.h"
#include "LotusLib/Compression.h"
#include "LotusLib/Enums.h"
#include "LotusLib/Exceptions.h"
#include "LotusLib/Logger.h" 
#include "LotusLib/PackageSplit.h"
#include "LotusLib/TOCTree.h"
#include "LotusLib/Utils.h"

#include <filesystem>
#include <fstream>
#include <mutex>
#include <vector>

using namespace LotusLib;

namespace LotusLib
{
	namespace Impl
	{
		struct PackageSplitState
		{
			TOCTree tocTree;
			bool cacheExists;
			// Windows has abysmal performance for creating file handles.
			// So if `getDataAndDecompress` is called on every file in the package (~200K for Misc), the performance tanks.
			std::ifstream cacheReader;
			std::mutex decompressMutex;

			PackageSplitState(std::filesystem::path tocPath, std::filesystem::path cachePath);
		};
	};

	class PackageSplit
	{
		static const unsigned int m_magicNumber = 0x1867C64E;
		static const unsigned int m_archiveVersion1 = 16;
		static const unsigned int m_archiveVersion2 = 20;

		Game m_game;
		PkgSplitType m_pkgSplit;
		std::filesystem::path m_tocPath;
		std::filesystem::path m_cachePath;

		std::shared_ptr<Impl::PackageSplitState> m_state;
		std::shared_ptr<LotusLib::Impl::CompressionScratch> m_compressScratch;
		std::shared_ptr<LotusLib::Impl::CompressionScratch> m_commonheaderScratch;

	public:
		PackageSplit(std::filesystem::path tocPath, std::filesystem::path cachePath, Game game, PkgSplitType pkgSplit);

		Game getGame() const;
		const std::filesystem::path& getTocPath() const;
		const std::filesystem::path& getCachePath() const;

		std::vector<FileNode>::const_iterator begin() const;
		std::vector<FileNode>::const_iterator end() const;
		TOCTreeIterator getIter(const std::string& path) const;
		TOCTreeIterator getIter() const;

		bool fileExists(const std::string& internalPath) const;
		bool dirExists(const std::string& internalPath) const;
		bool fileExists(const FileNode& fileNode) const;
		bool dirExists(const DirNode& dirNode) const;

		const FileNode& getFileNode(const FileNode& fileNode) const;
		const DirNode& getDirNode(const DirNode& dirNode) const;

		const FileNode& getFileNode(const std::string& internalPath) const;
		const DirNode& getDirNode(const std::string& internalPath) const;

		size_t dirCount() const;
		size_t fileCount() const;
		size_t fileDupeCount() const;

		// FileNode MUST be from this PackageSplit
		std::vector<uint8_t> getFileUncompressed(const std::string& internalPath) const;
		std::vector<uint8_t> getFileUncompressed(const FileNode& entry) const;
		std::vector<uint8_t> getFile(const std::string& internalPath) const;
		std::vector<uint8_t> getFile(const FileNode& entry) const;

		// Can only be called inside the HEADER PkgSplit
		CommonHeader readCommonHeader(const FileNode& entry);
		uint32_t readCommonHeaderFormat(const FileNode& entry);

	private:
		void decompress(const FileNode& entry, uint8_t* outData) const;
		bool _isTOCValid() const;
	};
}
