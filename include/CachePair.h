#pragma once

#include "DirectoryTree.h"
#include "LotusExceptions.h"
#include "LotusPath.h"
#include "Compression.h"
#include "Enums.h"

#include <filesystem>
#include <fstream>
#include <string>

namespace LotusLib
{
	class CachePair
	{
	protected:
		static const unsigned int m_magicNumber = 0x1867C64E;
		static const unsigned int m_archiveVersion1 = 16;
		static const unsigned int m_archiveVersion2 = 20;
		Game m_game;
		std::filesystem::path m_tocPath;
		std::filesystem::path m_cachePath;
		DirectoryTree m_dirTree;
		bool m_hasReadToc;
		// Windows has abysmal performance for creating file handles
		// So if `getDataAndDecompress` is called on every file in the package (~200K for Misc)...
		// The performance tanks
		mutable std::ifstream m_cacheReader;

	public:
		CachePair(std::filesystem::path tocPath, std::filesystem::path cachePath, Game game);

		void readToc();
		void unReadToc();

		Game getGame() const;
		const std::filesystem::path& getTocPath() const;
		const std::filesystem::path& getCachePath() const;

		DirectoryTree::ConstFileIterator begin() const;
		DirectoryTree::ConstFileIterator end() const;
		DirectoryTree::FileIteratorTree getIter(const LotusPath& path) const;
		DirectoryTree::FileIteratorTree getIter() const;
		
		FileNode* getFileEntry(const LotusPath& lotusPath);
		DirNode* getDirEntry(const LotusPath& lotusPath);
		const FileNode* getFileEntry(const LotusPath& lotusPath) const;
		const DirNode* getDirEntry(const LotusPath& lotusPath) const;

		size_t dirCount() const;
		size_t fileCount() const;
		size_t fileDupeCount() const;

		void lsDir(const LotusPath& internalPath) const;

		std::vector<uint8_t> getData(const LotusPath& internalPath) const;
		std::vector<uint8_t> getDataAndDecompress(const LotusPath& internalPath) const;
		std::vector<uint8_t> getData(const FileEntries::FileNode* entry) const;
		std::vector<uint8_t> getDataAndDecompress(const FileEntries::FileNode* entry) const;

	private:
		bool isValid(std::ifstream& tocReader) const;
	};
}
