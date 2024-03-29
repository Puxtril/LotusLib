#pragma once

#include "DirectoryTree.h"
#include "LotusExceptions.h"
#include "LotusPath.h"

#include <filesystem>
#include <fstream>
#include <string>

namespace LotusLib
{
	//! \brief Base class for other CachePairs
	//! 
	//! H.Texture.toc
	//!           \---
	//!            |
	//!       You are here
	class CachePair
	{
	protected:
		static const unsigned int m_magicNumber = 0x1867C64E;
		static const unsigned int m_archiveVersion = 20;
		bool m_isPostEnsmallening;
		std::filesystem::path m_tocPath;
		std::filesystem::path m_cachePath;
		DirectoryTree m_dirTree;
		bool m_hasReadToc;
		LotusLib::Logger& m_logger;

	public:
		CachePair(std::filesystem::path tocPath, std::filesystem::path cachePath, bool isPostEnsmallening);

		CachePair(const CachePair&) = delete;
		CachePair& operator=(CachePair&) = delete;

		void readToc();
		void unReadToc();

		bool isPostEnsmallening() const;
		const std::filesystem::path& getTocPath() const;
		const std::filesystem::path& getCachePath() const;

		DirectoryTree::ConstFileIterator begin() const;
		DirectoryTree::ConstFileIterator end() const;
		DirectoryTree::ConstFileIteratorTree getIter(const LotusPath& path) const;
		DirectoryTree::ConstFileIteratorTree getIter() const;
		
		FileNode* getFileEntry(const LotusPath& lotusPath);
		DirNode* getDirEntry(const LotusPath& lotusPath);
		const FileNode* getFileEntry(const LotusPath& lotusPath) const;
		const DirNode* getDirEntry(const LotusPath& lotusPath) const;

		size_t dirCount() const;
		size_t fileCount() const;
		size_t fileDupeCount() const;

		void lsDir(const LotusPath& internalPath) const;

	private:
		bool isValid(std::ifstream& tocReader) const;

	};
}
