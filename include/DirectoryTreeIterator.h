#pragma once

#include "DirNode.h"
#include "FileNode.h"

#include <stack>

namespace LotusLib
{
	class DirectoryTreeIterator {
	private:
		const FileEntries::DirNode* m_startingPos;
		const FileEntries::DirNode* m_ptr;
		std::stack<uint32_t> m_dirPositions;
		int m_filePosition;

		using iterator_category = std::input_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = const FileEntries::FileNode*;
		using pointer = value_type*;
		using reference = value_type&;

		friend class DirectoryTree;

		DirectoryTreeIterator();
		DirectoryTreeIterator(const FileEntries::DirNode* ptr);

	public:
		DirectoryTreeIterator& operator++();
		const DirectoryTreeIterator	operator++(int);
		const FileEntries::FileNode* operator*() const;
		bool operator==(const DirectoryTreeIterator& other) const;
		bool operator!=(const DirectoryTreeIterator& other) const;

		// Depth of the current directory
		int depth() const;

		std::string getFullPath() const;

		const FileEntries::DirNode*& getCurDir();

	private:
		// Return true if at the end
		bool findNextValidDir();

		//! Returns true if the current state is valid
		//! Typical input state: m_ptr.getSubDir(m_dirPositions.top()) has not been visited yet (currently invalid)
		//!
		//! Possible output states:
		//! 1: Enters directory with files and no subdirs (valid state)
		//! 2: Enters directory with no files and no subdirs (invalid state)
		//! 3 (similar to 1): Does not leave current directory, m_dirPostions.top() >= m_ptr->getDirCount() (valid state)
		//!		- This method should not be called in this case, because nothing will happen
		bool recurseToLowestFromCurrent();

		//! Returns true if the current state is valid
		//! Typical input state: m_filePostion >= m_ptr->getFileCount() (currently invalid)
		//
		//! Possible output states:
		//! 1: Finds directory, m_dirPositions.top() < m_ptr->getDirCount() (invalid state, needs to recurse down)
		//! 2: Finds directory, m_dirPositions.top() >= m_ptr->getDirCount() && m_ptr->getFileCount() > 0 (valid state, all dirs have been visited)
		//! 3: Does not find directory, stack is empty (end state)
		bool recurseUpToNextDir();

		void getPathRecursive(std::stringstream& stream, const FileEntries::DirNode* ptr) const;
	};
}
