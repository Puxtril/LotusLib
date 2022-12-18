#include "FileNode.h"
#include "DirNode.h"

using namespace LotusLib::FileEntries;

FileNode::FileNode()
	: m_name(std::string()),
	m_parentDir(nullptr),
	m_cacheOffset(UINT64_MAX),
	m_timeStamp(0),
	m_compLen(0),
	m_len(0),
	m_tocOffset(0)
{
}

FileNode::FileNode(std::string name, DirNode* parentDir, int64_t offset, int64_t timeStamp, int32_t compressedLength, int32_t length, int tocOffset)
	: m_name(name),
	m_parentDir(parentDir),
	m_cacheOffset(offset),
	m_timeStamp(timeStamp),
	m_compLen(compressedLength),
	m_len(length),
	m_tocOffset(tocOffset)
{
}

void
FileNode::setData(std::string name, DirNode* parentDir, int64_t offset, int64_t timeStamp, int32_t compressedLength, int32_t length, int tocOffset)
{
	m_name = name;
	m_parentDir = parentDir;
	m_cacheOffset = offset;
	m_timeStamp = timeStamp;
	m_compLen = compressedLength;
	m_len = length;
	m_tocOffset = tocOffset;
}

const std::string&
FileNode::getName() const
{
	return m_name;
}

const DirNode*
FileNode::getParent() const
{
	return m_parentDir;
}

int32_t
FileNode::getLen() const
{
	return m_len;
}

int32_t
FileNode::getCompLen() const
{
	return m_compLen;
}

int64_t
FileNode::getOffset() const
{
	return m_cacheOffset;
}

int64_t
FileNode::getTimeStamp() const
{
	return m_timeStamp;
}

int
FileNode::getTocOffset() const
{
	return m_tocOffset;
}

std::string
FileNode::getFullPath() const
{
	std::stringstream fullPath = m_parentDir->getFullpathStream();
	fullPath << "/" << m_name;
	return fullPath.str();
}