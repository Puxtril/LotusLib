#pragma once

#include "LotusLib/Exceptions.h"
#include "LotusLib/Logger.h"
#include "LotusLib/TOCEntry.h"

#include <cstdint>
#include <fstream>
#include <filesystem>
#include <stack>
#include <string>
#include <vector>

namespace LotusLib
{
	struct DirNode;
	class TOCTreeIterator;

	////////////////////////////////////////////////////////////////////////////////
	// Nodes

	struct FileNode
    {
		std::string name;
		DirNode* parentDir;
		int64_t cacheOffset;
		int64_t timeStamp;
		int32_t compLen;
		int32_t len;
		int tocEntryIndex;
    };


    struct DirNode
    {
		std::string name;
		DirNode* parentNode;
		int tocEntryIndex;
		std::vector<DirNode*> childDirs;
		std::vector<FileNode*> childFiles;
		std::vector<FileNode*> childFileDupes;
    };

	////////////////////////////////////////////////////////////////////////////////
	// Tree

	class TOCTree
	{
		bool m_isInitilized;
		std::filesystem::path m_tocPath;

		DirNode* m_rootNode;

		std::vector<DirNode> m_dirs;
		std::vector<FileNode> m_files;
		std::vector<FileNode> m_filesDupes;

	public:
		TOCTree() = default;
		TOCTree(const std::filesystem::path& tocPath);

		void initialize();

		std::vector<FileNode>::const_iterator begin() const;
		std::vector<FileNode>::const_iterator end() const;
		TOCTreeIterator getIter(const std::string& path) const;
		TOCTreeIterator getIter() const;

		size_t dirCount() const;
		size_t fileCount() const;
		size_t fileDupeCount() const;

		bool fileExists(const std::string& internalPath) const;
		bool dirExists(const std::string& internalPath) const;
		bool fileExists(const FileNode& fileNode) const;
		bool dirExists(const DirNode& dirNode) const;

		const FileNode& getFileNode(const std::string& internalPath) const;
		const DirNode& getDirNode(const std::string& internalPath) const;
		const FileNode& getFileNode(const FileNode& fileNode) const;
		const DirNode& getDirNode(const DirNode& dirNode) const;

	private:
		const DirNode* findChildDir(const DirNode* dirNode, const std::string& path, size_t start, size_t len) const;
		const FileNode* findChildFile(const DirNode* dirNode, const std::string& path, size_t start, size_t len) const;
		const DirNode* findChildDir(const DirNode* dirNode, const std::string& name) const;
		const FileNode* findChildFile(const DirNode* dirNode, const std::string& name) const;
		void readToc(const std::filesystem::path& tocPath);
	};

	////////////////////////////////////////////////////////////////////////////////
	// Iterator

	class TOCTreeIterator {
	private:
		const DirNode* m_startingPos;
		const DirNode* m_ptr;
		std::stack<uint32_t> m_dirPositions;
		int m_filePosition;

		using iterator_category = std::input_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = const FileNode;
		using pointer = value_type*;
		using reference = value_type&;

		friend class PackageSplit;

	public:
		TOCTreeIterator();
		TOCTreeIterator(const DirNode* ptr);

		TOCTreeIterator& operator++();
		const TOCTreeIterator	operator++(int);
		const FileNode& operator*() const;
		bool operator==(const TOCTreeIterator& other) const;
		bool operator!=(const TOCTreeIterator& other) const;

		// Depth of the current directory
		int depth() const;

		const DirNode* getCurDir();

	private:
		// Return true if at the end
		bool findNextValidDir();

		// Returns true if the current state is valid
		// Typical input state: m_ptr.getSubDir(m_dirPositions.top()) has not been visited yet (currently invalid)
		//
		// Possible output states:
		// 1: Enters directory with files and no subdirs (valid state)
		// 2: Enters directory with no files and no subdirs (invalid state)
		// 3 (similar to 1): Does not leave current directory, m_dirPostions.top() >= m_ptr->getDirCount() (valid state)
		//		- This method should not be called in this case, because nothing will happen
		bool recurseToLowestFromCurrent();

		// Returns true if the current state is valid
		// Typical input state: m_filePostion >= m_ptr->getFileCount() (currently invalid)
		//
		// Possible output states:
		// 1: Finds directory, m_dirPositions.top() < m_ptr->getDirCount() (invalid state, needs to recurse down)
		// 2: Finds directory, m_dirPositions.top() >= m_ptr->getDirCount() && m_ptr->getFileCount() > 0 (valid state, all dirs have been visited)
		// 3: Does not find directory, stack is empty (end state)
		bool recurseUpToNextDir();
	};
}
