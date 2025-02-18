#include "DirectoryTreeIterator.h"

using namespace LotusLib;

DirectoryTreeIterator::DirectoryTreeIterator()
    : m_startingPos(nullptr), m_ptr(nullptr), m_dirPositions(std::stack<uint32_t>()), m_filePosition(0)
{}

DirectoryTreeIterator::DirectoryTreeIterator(const FileEntries::DirNode* ptr)
    : m_startingPos(ptr), m_ptr(ptr), m_dirPositions(std::stack<uint32_t>()), m_filePosition(0)
{
    m_dirPositions.push(0);
    ++(*this);
}

DirectoryTreeIterator&
DirectoryTreeIterator::operator++()
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

const DirectoryTreeIterator
DirectoryTreeIterator::operator++(int)
{
    DirectoryTreeIterator tmp(*this);
    ++(*this);
    return tmp;
}

const FileEntries::FileNode*
DirectoryTreeIterator::operator*() const
{
    return m_ptr->getChildFile(m_filePosition);
}

bool
DirectoryTreeIterator::operator==(const DirectoryTreeIterator& other) const
{
    return m_ptr == other.m_ptr && m_filePosition == other.m_filePosition;
}

bool
DirectoryTreeIterator::operator!=(const DirectoryTreeIterator& other) const
{
    return !(*this == other);
}

int
DirectoryTreeIterator::depth() const
{
    return (int)m_dirPositions.size() - 2;
}

std::string
DirectoryTreeIterator::getFullPath() const
{
    std::stringstream stream = std::stringstream();
    getPathRecursive(stream, m_ptr);
    stream << "/" << m_ptr->getChildFile(m_filePosition)->getName();
    return stream.str();
}

const FileEntries::DirNode*&
DirectoryTreeIterator::getCurDir()
{
    return m_ptr;
}

bool
DirectoryTreeIterator::findNextValidDir()
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

bool
DirectoryTreeIterator::recurseToLowestFromCurrent()
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

bool
DirectoryTreeIterator::recurseUpToNextDir()
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

void
DirectoryTreeIterator::getPathRecursive(std::stringstream& stream, const FileEntries::DirNode* ptr) const
{
    if (ptr->getParent() != nullptr)
    {
        getPathRecursive(stream, ptr->getParent());
        stream << "/" << ptr->getName();
    }
}