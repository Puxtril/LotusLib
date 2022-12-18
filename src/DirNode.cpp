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
DirNode::addChildFile(FileNode* node)
{
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

int
DirNode::findDupesRecursive()
{
	int* buffer = new int[4000];
	int dupeCount = this->findDupesRecursiveHelper(buffer);
	delete[] buffer;
	return dupeCount;
}

int
DirNode::findDupesRecursiveHelper(int* indiciesBuffer)
{
	int dupeIndex = 0;
	for (int x = 0; x < (int)m_childFiles.size() - 1; x++)
	{
		if (m_childFiles[x]->getTimeStamp() == 0)
			indiciesBuffer[dupeIndex++] = x;
	}

	for (int x = 0; x < dupeIndex; x++)
	{
		int index = indiciesBuffer[x] - x;
		m_childFileDupes.push_back(m_childFiles[index]);
		m_childFiles.erase(m_childFiles.begin() + index);
	}

	int totalDupes = 0;
	for (auto x : m_childDirs)
		totalDupes += x->findDupesRecursiveHelper(indiciesBuffer);

	return dupeIndex + totalDupes;
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
