#pragma once

#include "LotusLibLogger.h" 
#include "LotusExceptions.h"
#include "DirNode.h"
#include "FileNode.h"
#include "LotusPath.h"
#include "RawEntry.h"
#include "DirectoryTreeIterator.h"

#include <stdexcept>
#include <filesystem>
#include <vector>
#include <fstream>
#include <iostream>
#include <time.h>
#include <chrono>

#ifdef WIN32
// gmtime() warning supression
#pragma warning(disable : 4996)
#endif

using namespace LotusLib::FileEntries;

namespace LotusLib
{
	// This can be thought of as an interface for .toc files
	// This will not touch .cache files
	//
	// No method to iterate over directories
	// Two methods to iterate over files
	//  * FileIterator: Alias for std::vector<FileNode>::iterator
	//  * FileIteratorTree: Custom iterator that traverses the files in a depth-first algorithm
	class DirectoryTree
	{
		std::filesystem::path m_tocPath;
		std::vector<DirNode> m_dirs;
		std::vector<FileNode> m_files;
		std::vector<FileNode> m_filesDupes;
		DirNode* m_rootNode;
		bool m_hasRead;
		int m_dupeCount;

	public:
		using FileIteratorTree = DirectoryTreeIterator;
		using FileIterator = std::vector<FileNode>::iterator;
		using ConstFileIterator = std::vector<FileNode>::const_iterator;

		DirectoryTree(std::filesystem::path tocPath);

		void readTocFile();
		void unReadTocFile();

		FileIterator begin();
		FileIterator end();
		ConstFileIterator begin() const;
		ConstFileIterator end() const;

		FileIteratorTree getIter(const LotusPath& path);
		FileIteratorTree getIter();
		FileIteratorTree getIter(const LotusPath& path) const;
		FileIteratorTree getIter() const;

		FileNode* getFileEntry(const LotusPath& lotusPath);
		DirNode* getDirEntry(const LotusPath& lotusPath);
		const FileNode* getFileEntry(const LotusPath& lotusPath) const;
		const DirNode* getDirEntry(const LotusPath& lotusPath) const;

		size_t dirCount() const;
		size_t fileCount() const;
		size_t fileDupeCount() const;

		// Prints to stdout the directory in a similar fashion
		//  to the unix command `ls`
		void lsDir(const LotusPath& internalPath) const;
	};
}
