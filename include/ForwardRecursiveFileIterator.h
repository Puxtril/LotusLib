#pragma once

#include "DirNode.h"
#include "FileNode.h"

#include <stack>

namespace LotusLib
{
	//! \brief Iterates over files in a directory tree
	//!
	//! Does not provide access to directories, only files
	//! 
	//! This was a bit of an experiement for me, I didn't
	//!  want to write 2 of the same class, so ihe const and
	//!  non-const version were combined into this.
	template <bool Const = true>
	class ForwardRecursiveFileIterator {
	private:
		const FileEntries::DirNode* m_startingPos;
		const FileEntries::DirNode* m_ptr;
		std::stack<uint32_t> m_dirPositions;
		int m_filePosition;

		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = typename std::conditional_t<Const, const FileEntries::FileNode*, FileEntries::FileNode*>;
		using pointer = value_type*;
		using reference = value_type&;

		friend class DirectoryTree;

		ForwardRecursiveFileIterator()
			: m_startingPos(nullptr), m_ptr(nullptr), m_dirPositions(std::stack<uint32_t>()), m_filePosition(0)
		{}
		ForwardRecursiveFileIterator(const FileEntries::DirNode* ptr)
			: m_startingPos(ptr), m_ptr(ptr), m_dirPositions(std::stack<uint32_t>()), m_filePosition(0)
		{
			m_dirPositions.push(0);
			findNextValidDir();
		}

	public:
		ForwardRecursiveFileIterator&
		operator++()
		{
			if (m_filePosition++ < ((int32_t)m_ptr->getFileCount()) - 1)
			{
				return *this;
			}

			bool atEnd = findNextValidDir();
			m_filePosition = 0;

			if (atEnd)
			{
				m_ptr = nullptr;
				m_filePosition = 0;
			}

			return *this;
		}

		const ForwardRecursiveFileIterator
		operator++(int)
		{
			ForwardRecursiveFileIterator tmp(*this);
			++(*this);
			return tmp;
		}

		template <bool _Const = Const>
		std::enable_if_t< !_Const, value_type>
		operator*()
		{
			return m_ptr->getChildFile(m_filePosition);
		}

		template <bool _Const = Const>
		std::enable_if_t< _Const, value_type>
		operator*() const
		{
			return m_ptr->getChildFile(m_filePosition);
		}

		bool operator==(const ForwardRecursiveFileIterator& other) const
		{
			return m_ptr == other.m_ptr && m_filePosition == other.m_filePosition;
		}
		bool operator!=(const ForwardRecursiveFileIterator& other) const
		{
			return !(*this == other);
		}

		// Depth of the current directory
		int depth() const
		{
			return (int)m_dirPositions.size() - 2;
		}
		std::string getFullPath() const
		{
			std::stringstream stream = std::stringstream();
			getPathRecursive(stream, m_ptr);
			stream << "/" << m_ptr->getChildFile(m_filePosition)->getName();
			return stream.str();
		}

		template <bool _Const = Const>
		std::enable_if_t<!_Const, FileEntries::DirNode*&>
		getCurDir()
		{
			return m_ptr;
		}

		template <bool _Const = Const>
		std::enable_if_t<_Const, const FileEntries::DirNode*&>
		getCurDir() const
		{
			return m_ptr;
		}


	private:
		// Return true if at the end
		bool findNextValidDir()
		{
			bool isValid = false;

			while (!isValid)
			{
				isValid = recurseToLowestFromCurrent();
				if (!isValid)
					isValid = recurseUpToNextDir();
				if (m_dirPositions.size() < 1)
					return true;
			}

			return false;
		}

		//! Returns true if the current state is valid
		//! Typical input state: m_ptr.getSubDir(m_dirPositions.top()) has not been visited yet (currently invalid)
		//!
		//! Possible output states:
		//! 1: Enters directory with files and no subdirs (valid state)
		//! 2: Enters directory with no files and no subdirs (invalid state)
		//! 3 (similar to 1): Does not leave current directory, m_dirPostions.top() >= m_ptr->getDirCount() (valid state)
		//!		- This method should not be called in this case, because nothing will happen
		bool recurseToLowestFromCurrent()
		{
			uint32_t curSubdirIndex = m_dirPositions.top();
			m_dirPositions.pop();

			while (m_ptr->getDirCount() > curSubdirIndex)
			{
				m_dirPositions.push(curSubdirIndex);
				m_ptr = m_ptr->getChildDir(curSubdirIndex);
				curSubdirIndex = 0;
				m_filePosition = 0;
			}
			m_dirPositions.push(curSubdirIndex);

			if ((m_ptr->getFileCount() > 0 && (int)m_ptr->getFileCount() > m_filePosition))
				return true;
			return false;
		}

		//! Returns true if the current state is valid
		//! Typical input state: m_filePostion >= m_ptr->getFileCount() (currently invalid)
		//
		//! Possible output states:
		//! 1: Finds directory, m_dirPositions.top() < m_ptr->getDirCount() (invalid state, needs to recurse down)
		//! 2: Finds directory, m_dirPositions.top() >= m_ptr->getDirCount() && m_ptr->getFileCount() > 0 (valid state, all dirs have been visited)
		//! 3: Does not find directory, stack is empty (end state)
		bool recurseUpToNextDir()
		{
			uint32_t curSubdirIndex = m_dirPositions.top();
			m_dirPositions.pop();

			if (m_dirPositions.size() == 0)
				return false;
			do
			{
				m_ptr = m_ptr->getParent();
				curSubdirIndex = m_dirPositions.top() + 1;
				m_dirPositions.pop();
				m_filePosition = 0;
			} while (curSubdirIndex >= m_ptr->getDirCount() && m_ptr->getFileCount() == 0 && !m_dirPositions.empty());

			m_dirPositions.push(curSubdirIndex);

			if (curSubdirIndex <= m_ptr->getDirCount())
				return false;
			return true;
		}

		void getPathRecursive(std::stringstream& stream, const FileEntries::DirNode* ptr) const
		{
			if (ptr->getParent() != nullptr)
			{
				getPathRecursive(stream, ptr->getParent());
				stream << "/" << ptr->getName();
			}
		}
	};
}
