#include "LotusLib/TOCTree.h"

using namespace LotusLib;

////////////////////////////////////////////////////////////////////////////////
// Tree

TOCTree::TOCTree(const std::filesystem::path& tocPath)
{
    m_tocPath = tocPath;
    m_isInitilized = false;
}

void
TOCTree::initialize()
{
    if (!m_isInitilized)
    {
        readToc(m_tocPath);
        m_isInitilized = true;
    }
}

std::vector<FileNode>::const_iterator
TOCTree::begin() const
{
    return m_files.begin();
}

std::vector<FileNode>::const_iterator
TOCTree::end() const
{
    return m_files.end();
}

TOCTreeIterator
TOCTree::getIter(const std::string& path) const
{
    return TOCTreeIterator(&getDirNode(path));
}

TOCTreeIterator
TOCTree::getIter() const
{
    return TOCTreeIterator();
}

size_t
TOCTree::dirCount() const
{
	return m_dirs.size();
}

size_t
TOCTree::fileCount() const
{
	return m_files.size();
}

size_t
TOCTree::fileDupeCount() const
{
	return m_filesDupes.size();
}

bool
TOCTree::fileExists(const std::string& internalPath) const
{
    try
    {
        getFileNode(internalPath);
        return true;
    }
    catch (InternalEntryNotFound&)
    {
        return false;
    }
}

bool
TOCTree::dirExists(const std::string& internalPath) const
{
    try
    {
        getDirNode(internalPath);
        return true;
    }
    catch (InternalEntryNotFound&)
    {
        return false;
    }
}

bool
TOCTree::fileExists(const FileNode& fileNode) const
{
    try
    {
        getFileNode(fileNode);
        return true;
    }
    catch (InternalEntryNotFound&)
    {
        return false;
    }
}

bool
TOCTree::dirExists(const DirNode& dirNode) const
{
    try
    {
        getDirNode(dirNode);
        return true;
    }
    catch (InternalEntryNotFound&)
    {
        return false;
    }
}

const FileNode&
TOCTree::getFileNode(const std::string& internalPath) const
{
	const DirNode* dirPos = m_rootNode;
	const DirNode* nextDirPos;
	size_t curStart = 0, curEnd = 0;

	if (internalPath[0] == '/')
		curEnd = curStart = 1;

	while (true)
	{
		curEnd = internalPath.find('/', curStart);
		if (curEnd == std::string::npos)
		{
			const FileNode* fNode = findChildFile(dirPos, internalPath, curStart, curEnd);
			if (fNode != nullptr)
				return *fNode;
			throw InternalFileNotFound("\"" + dirPos->name + "\" doesn't contain file \"" + internalPath.substr(curStart, curEnd - curStart) + "\"");
		}
		nextDirPos = findChildDir(dirPos, internalPath, curStart, curEnd - curStart);
		if (nextDirPos == nullptr)
			throw InternalDirectoryNotFound("\"" + dirPos->name + "\" doesn't contain directory \"" + internalPath.substr(curStart, curEnd - curStart) + "\"");
		dirPos = nextDirPos;
		curStart = curEnd + 1;
	}

    throw InternalFileNotFound(internalPath);
}

const DirNode&
TOCTree::getDirNode(const std::string& internalPath) const
{
	const DirNode* dirPos = m_rootNode;
	const DirNode* nextDirPos;
	size_t curStart = 0, curEnd = 0;

	if (internalPath.length() == 1 && internalPath[0] == '/')
		return *m_rootNode;

	if (internalPath[0] == '/')
		curEnd = curStart = 1;

	while (true)
	{
		curEnd = internalPath.find('/', curStart);
		nextDirPos = findChildDir(dirPos, internalPath, curStart, curEnd - curStart);
		if (nextDirPos == nullptr)
			throw InternalDirectoryNotFound("\"" + dirPos->name + "\" doesn't contain directory \"" + internalPath.substr(curStart, curEnd - curStart) + "\"");
		if (curEnd == std::string::npos)
			return *nextDirPos;
		dirPos = nextDirPos;
		curStart = curEnd + 1;
	}

    throw InternalFileNotFound(internalPath);
}

const FileNode&
TOCTree::getFileNode(const FileNode& fileNode) const
{
    std::stack<const DirNode*> pathStack;
    const DirNode* ptr = fileNode.parentDir;

    while (ptr->parentNode != nullptr)
    {
        pathStack.push(ptr);
        ptr = ptr->parentNode;
    }

    while (!pathStack.empty())
    {
        ptr = findChildDir(ptr, pathStack.top()->name);
        if (ptr == nullptr)
            throw InternalDirectoryNotFound(ptr->name + " does not contain directory " + pathStack.top()->name);
        pathStack.pop();
    }

    const FileNode* foundNode = findChildFile(ptr, fileNode.name);
    if (foundNode == nullptr)
        throw InternalFileNotFound(ptr->name + " does not contain directory " + fileNode.name);

    return *foundNode;
}

const DirNode&
TOCTree::getDirNode(const DirNode& dirNode) const
{
    std::stack<const DirNode*> pathStack;
    const DirNode* ptr = &dirNode;

    while (ptr->parentNode != nullptr)
    {
        pathStack.push(ptr);
        ptr = ptr->parentNode;
    }

    while (!pathStack.empty())
    {
        ptr = findChildDir(ptr, pathStack.top()->name);
        if (ptr == nullptr)
            throw InternalDirectoryNotFound(ptr->name + " does not contain directory " + pathStack.top()->name);
        pathStack.pop();
    }

    return *ptr;
}

const DirNode*
TOCTree::findChildDir(const DirNode* dirNode, const std::string& path, size_t start, size_t len) const
{
	for (DirNode* curNode : dirNode->childDirs)
	{
		if (path.compare(start, len, curNode->name) == 0)
			return curNode;
	}
	return nullptr;
}

const FileNode*
TOCTree::findChildFile(const DirNode* dirNode, const std::string& path, size_t start, size_t len) const
{
	for (FileNode* curNode : dirNode->childFiles)
	{
		if (path.compare(start, len, curNode->name) == 0)
			return curNode;
	}
	return nullptr;
}

const DirNode*
TOCTree::findChildDir(const DirNode* dirNode, const std::string& name) const
{
    for (DirNode* curNode : dirNode->childDirs)
	{
		if (name.compare(curNode->name) == 0)
			return curNode;
	}
	return nullptr;
}

const FileNode*
TOCTree::findChildFile(const DirNode* dirNode, const std::string& name) const
{
    for (FileNode* curNode : dirNode->childFiles)
	{
		if (name.compare(curNode->name) == 0)
			return curNode;
	}
	return nullptr;
}

void
TOCTree::readToc(const std::filesystem::path& tocPath)
{
	const static int TOC_ENTRY_LEN = sizeof(RawTOCEntry);

	std::ifstream tocReader(tocPath, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
	int entryCount = static_cast<int>((tocReader.tellg() - (std::streampos)8) / (std::streampos)TOC_ENTRY_LEN);
	tocReader.seekg(8, std::ios_base::beg);

    std::vector<RawTOCEntry> rawEntries(entryCount);
    tocReader.read((char*)rawEntries.data(), TOC_ENTRY_LEN * entryCount);

	int fileDupeCount = 0, fileCount = 0, dirCount = 1;
    for (int i = 0; i < entryCount; i++)
    {
        if (rawEntries[i].cacheOffset == -1)
            dirCount++;
        else if (rawEntries[i].timeStamp == 0)
            fileDupeCount++;
        else
            fileCount++;
    }

    m_filesDupes.resize(fileDupeCount);
	m_files.resize(fileCount);
	m_dirs.resize(dirCount);

    fileDupeCount = 0, fileCount = 0, dirCount = 1;
	for (int i = 0; i < entryCount; i++)
	{
        RawTOCEntry& entryBuffer = rawEntries[i];

		if (entryBuffer.cacheOffset == -1)
		{
            DirNode& curNode = m_dirs[dirCount];
			curNode.name = std::string(entryBuffer.name);
            curNode.parentNode = &m_dirs[entryBuffer.parentDirIndex];
            curNode.tocEntryIndex = i;

            m_dirs[entryBuffer.parentDirIndex].childDirs.push_back(&m_dirs[dirCount]);
            dirCount++;
		}
		else
		{
			if (entryBuffer.timeStamp == 0)
			{
                FileNode& curNode = m_filesDupes[fileDupeCount];
                curNode.name = std::string(entryBuffer.name);
                curNode.parentDir = &m_dirs[entryBuffer.parentDirIndex];
                curNode.cacheOffset = entryBuffer.cacheOffset;
                curNode.timeStamp = entryBuffer.timeStamp;
                curNode.compLen = entryBuffer.compressedLen;
                curNode.len = entryBuffer.length;
                curNode.tocEntryIndex = i;
                
                m_dirs[entryBuffer.parentDirIndex].childFileDupes.push_back(&m_filesDupes[fileDupeCount]);
                fileDupeCount++;
			}

			else
			{
                FileNode& curNode = m_files[fileCount];
                curNode.name = std::string(entryBuffer.name);
                curNode.parentDir = &m_dirs[entryBuffer.parentDirIndex];
                curNode.cacheOffset = entryBuffer.cacheOffset;
                curNode.timeStamp = entryBuffer.timeStamp;
                curNode.compLen = entryBuffer.compressedLen;
                curNode.len = entryBuffer.length;
                curNode.tocEntryIndex = i;

                m_dirs[entryBuffer.parentDirIndex].childFiles.push_back(&m_files[fileCount]);
                fileCount++;
			}
		}
	}

    m_dirs[0].parentNode = nullptr;
    m_dirs[0].tocEntryIndex = 0;
    m_rootNode = &m_dirs[0];
}

////////////////////////////////////////////////////////////////////////////////
// Iterator

TOCTreeIterator::TOCTreeIterator()
    : m_startingPos(nullptr), m_ptr(nullptr), m_dirPositions(std::stack<uint32_t>()), m_filePosition(0)
{}

TOCTreeIterator::TOCTreeIterator(const DirNode* ptr)
    : m_startingPos(ptr), m_ptr(ptr), m_dirPositions(std::stack<uint32_t>()), m_filePosition(0)
{
    m_dirPositions.push(0);
    
    bool atEnd = findNextValidDir();
    if (atEnd)
    {
        m_ptr = nullptr;
        m_filePosition = 0;
    }
}

TOCTreeIterator&
TOCTreeIterator::operator++()
{
    if (m_filePosition++ < ((int32_t)m_ptr->childFiles.size()) - 1)
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

const TOCTreeIterator
TOCTreeIterator::operator++(int)
{
    TOCTreeIterator tmp(*this);
    ++(*this);
    return tmp;
}

const FileNode&
TOCTreeIterator::operator*() const
{
    return *m_ptr->childFiles[m_filePosition];
}

bool
TOCTreeIterator::operator==(const TOCTreeIterator& other) const
{
    return m_ptr == other.m_ptr && m_filePosition == other.m_filePosition;
}

bool
TOCTreeIterator::operator!=(const TOCTreeIterator& other) const
{
    return !(*this == other);
}

int
TOCTreeIterator::depth() const
{
    return (int)m_dirPositions.size() - 2;
}

const DirNode*
TOCTreeIterator::getCurDir()
{
    return m_ptr;
}

bool
TOCTreeIterator::findNextValidDir()
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
TOCTreeIterator::recurseToLowestFromCurrent()
{
    uint32_t curSubdirIndex = m_dirPositions.top();
    m_dirPositions.pop();

    while (m_ptr->childDirs.size() > curSubdirIndex)
    {
        m_dirPositions.push(curSubdirIndex);
        m_ptr = m_ptr->childDirs[curSubdirIndex];
        curSubdirIndex = 0;
        m_filePosition = 0;
    }
    m_dirPositions.push(curSubdirIndex);

    if ((m_ptr->childFiles.size() > 0 && (int)m_ptr->childFiles.size() > m_filePosition))
        return true;
    return false;
}

bool
TOCTreeIterator::recurseUpToNextDir()
{
    uint32_t curSubdirIndex = m_dirPositions.top();
    m_dirPositions.pop();

    if (m_dirPositions.size() == 0)
        return false;
    do
    {
        m_ptr = m_ptr->parentNode;
        curSubdirIndex = m_dirPositions.top() + 1;
        m_dirPositions.pop();
        m_filePosition = 0;
    } while (curSubdirIndex >= m_ptr->childDirs.size() && m_ptr->childFiles.size() == 0 && !m_dirPositions.empty());

    m_dirPositions.push(curSubdirIndex);

    if (curSubdirIndex <= m_ptr->childDirs.size())
        return false;
    return true;
}
