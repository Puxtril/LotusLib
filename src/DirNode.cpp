#include "DirNode.h"

using namespace LotusLib::FileEntries;

DirNode::DirNode()
	: m_name(""),
	m_parentNode(nullptr),
	m_tocOffset(0),
	m_childDirs(std::vector<DirNode*>()),
	m_childFiles(std::vector<FileNode*>()),
	m_childFileDupes(std::vector<FileNode*>())
{ }

DirNode::DirNode(DirNode* parentNode, std::string name, int tocOffset)
	: m_name(name),
	m_parentNode(parentNode),
	m_tocOffset(tocOffset),
	m_childDirs(std::vector<DirNode*>()),
	m_childFiles(std::vector<FileNode*>()),
	m_childFileDupes(std::vector<FileNode*>())
{ }

void
DirNode::setData(DirNode* parentNode, std::string name, int tocOffset)
{
	m_parentNode = parentNode;
	m_name = name;
	m_tocOffset = tocOffset;
}

const std::string&
DirNode::getName() const
{
	return m_name;
}

const DirNode*
DirNode::getParent() const
{
	return m_parentNode;
}

int
DirNode::getTocOffset() const
{
	return m_tocOffset;
}

size_t
DirNode::getDirCount() const
{
	return m_childDirs.size();
}

std::string
DirNode::getFullPath() const
{
	std::stringstream stream = std::stringstream();
	getPathRecursive(stream);
	return stream.str();
}

std::stringstream
DirNode::getFullpathStream() const
{
	std::stringstream stream = std::stringstream();
	getPathRecursive(stream);
	return stream;
}

size_t
DirNode::getFileCount() const
{
	return m_childFiles.size();
}

const DirNode*
DirNode::getChildDir(int index) const
{
	try
	{
		return m_childDirs.at(index);
	}
	catch (std::out_of_range&)
	{
		return nullptr;
	}
}

const FileNode*
DirNode::getChildFile(int index) const
{
	try
	{
		return m_childFiles.at(index);
	}
	catch (std::out_of_range&)
	{
		return nullptr;
	}
}

void
DirNode::addChildDir(DirNode* node)
{
	m_childDirs.push_back(node);
}

void
DirNode::addChildFile(FileNode* node, bool isDupe)
{
	if (isDupe)
		m_childFileDupes.push_back(node);
	else
		m_childFiles.push_back(node);
}

const DirNode*
DirNode::getChildDir(const std::string& name) const
{
	for (DirNode* curNode : m_childDirs)
	{
		if (name.compare(curNode->getName()) == 0)
			return curNode;
	}
	return nullptr;
}

const FileNode*
DirNode::getChildFile(const std::string& name) const
{
	for (FileNode* curNode : m_childFiles)
	{
		if (name.compare(curNode->getName()) == 0)
			return curNode;
	}
	return nullptr;
}

const DirNode*
DirNode::findChildDir(const std::string& path, size_t start, size_t len) const
{
	for (DirNode* curNode : m_childDirs)
	{
		if (path.compare(start, len, curNode->getName()) == 0)
			return curNode;
	}
	return nullptr;
}

const FileNode*
DirNode::findChildFile(const std::string& path, size_t start, size_t len) const
{
	for (FileNode* curNode : m_childFiles)
	{
		if (path.compare(start, len, curNode->getName()) == 0)
			return curNode;
	}
	return nullptr;
}

void
DirNode::getPathRecursive(std::stringstream& stream) const
{
	if (m_parentNode != nullptr)
	{
		m_parentNode->getPathRecursive(stream);
		stream << "/" << m_name;
	}
}
