#include "DirectoryTree.h"

using namespace LotusLib;

DirectoryTree::DirectoryTree(std::filesystem::path tocPath)
	: m_log(LotusLib::Logger::getInstance()),
	m_tocPath(tocPath),
	m_dirs(std::vector<DirNode>()),
	m_files(std::vector<FileNode>()),
	m_rootNode(nullptr),
	m_hasRead(false),
	m_dupeCount(-1)
{
}

void
DirectoryTree::readTocFile()
{
	if (m_hasRead)
		return;

	const static int TOC_ENTRY_LEN = sizeof(RawTOCEntry);

	std::ifstream tocReader(m_tocPath, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
	int entryCount = static_cast<int>((tocReader.tellg() - (std::streampos)8) / (std::streampos)TOC_ENTRY_LEN);
	tocReader.seekg(8, std::ios_base::beg);

	RawTOCEntry entryBuffer;
	m_files.resize(entryCount);
	m_dirs.resize(entryCount);
	int realFileCount = 0;
	int realDirCount = 1;

	for (int i = 0; i < entryCount; i++)
	{
		tocReader.read((char*)&entryBuffer, TOC_ENTRY_LEN);
		if (entryBuffer.cacheOffset == -1)
		{
			m_dirs[realDirCount].setData(
				&m_dirs[entryBuffer.parentDirIndex],
				std::string(entryBuffer.name),
				i
			);
			m_dirs[entryBuffer.parentDirIndex].addChildDir(&m_dirs[realDirCount]);
			realDirCount++;
		}
		else
		{
			m_files[realFileCount].setData(
				std::string(entryBuffer.name),
				&m_dirs[entryBuffer.parentDirIndex],
				entryBuffer.cacheOffset,
				entryBuffer.timeStamp,
				entryBuffer.compressedLen,
				entryBuffer.length,
				i
			);
			m_dirs[entryBuffer.parentDirIndex].addChildFile(&m_files[realFileCount]);
			realFileCount++;
		}
	}
	m_files.resize(realFileCount);
	m_dirs.resize(realDirCount);
	m_rootNode = &m_dirs[0];

	m_hasRead = true;
	m_log.info(spdlog::fmt_lib::format("{} files, {} dirs", realFileCount, realDirCount));
}

void
DirectoryTree::unReadTocFile()
{
	m_dirs.clear();
	m_files.clear();
	m_rootNode = nullptr;
	m_dupeCount = -1;
	m_hasRead = false;
}

int
DirectoryTree::findDuplicates()
{
	if (m_dupeCount != -1)
		return m_dupeCount;
	m_dupeCount = m_rootNode->findDupesRecursive();
	m_log.info(spdlog::fmt_lib::format("Found {} duplicates", m_dupeCount));
	return m_dupeCount;
}

DirectoryTree::FileIterator
DirectoryTree::begin()
{
	return m_files.begin();
}

DirectoryTree::FileIterator
DirectoryTree::end()
{
	return m_files.end();
}

DirectoryTree::ConstFileIterator
DirectoryTree::begin() const
{
	return m_files.begin();
}

DirectoryTree::ConstFileIterator
DirectoryTree::end() const
{
	return m_files.end();
}

DirectoryTree::FileIteratorTree
DirectoryTree::getIter(const LotusPath& path)
{
	return DirectoryTree::FileIteratorTree(getDirEntry(path));
}

DirectoryTree::FileIteratorTree
DirectoryTree::getIter()
{
	return DirectoryTree::FileIteratorTree();
}

DirectoryTree::ConstFileIteratorTree
DirectoryTree::getIter(const LotusPath& path) const
{
	return DirectoryTree::ConstFileIteratorTree(getDirEntry(path));
}

DirectoryTree::ConstFileIteratorTree
DirectoryTree::getIter() const
{
	return DirectoryTree::ConstFileIteratorTree();
}

FileNode*
DirectoryTree::getFileEntry(const LotusPath& lotusPath)
{
	return const_cast<FileNode*>(const_cast<const DirectoryTree*>(this)->getFileEntry(lotusPath));
}

DirNode*
DirectoryTree::getDirEntry(const LotusPath& lotusPath)
{
	return const_cast<DirNode*>(const_cast<const DirectoryTree*>(this)->getDirEntry(lotusPath));
}

const FileNode*
DirectoryTree::getFileEntry(const LotusPath& lotusPath) const
{
	std::string lotusPathStr(lotusPath.string());

	const DirNode* dirPos = m_rootNode;
	size_t curStart = 0, curEnd = 0;

	if (lotusPathStr[0] == '/')
		curEnd = curStart = 1;

	while (true)
	{
		curEnd = lotusPathStr.find('/', curStart);
		if (curEnd == std::string::npos)
		{
			const FileNode* fNode = dirPos->findChildFile(lotusPathStr, curStart, curEnd);
			if (fNode != nullptr)
				return fNode;
			throw std::filesystem::filesystem_error(lotusPathStr, std::make_error_code(std::errc::no_such_file_or_directory));
		}
		dirPos = dirPos->findChildDir(lotusPathStr, curStart, curEnd - curStart);
		if (dirPos == nullptr)
			throw std::filesystem::filesystem_error(lotusPathStr, std::make_error_code(std::errc::no_such_file_or_directory));
		curStart = curEnd + 1;
	}

	return nullptr;
}

const DirNode*
DirectoryTree::getDirEntry(const LotusPath& lotusPath) const
{
	std::string lotusPathStr(lotusPath.string());

	const DirNode* dirPos = m_rootNode;
	size_t curStart = 0, curEnd = 0;

	if (lotusPathStr.length() == 1 && lotusPathStr[0] == '/')
		return m_rootNode;

	if (lotusPathStr[0] == '/')
		curEnd = curStart = 1;

	while (true)
	{
		curEnd = lotusPathStr.find('/', curStart);
		dirPos = dirPos->findChildDir(lotusPathStr, curStart, curEnd - curStart);
		if (dirPos == nullptr)
			throw std::filesystem::filesystem_error(lotusPathStr, std::make_error_code(std::errc::no_such_file_or_directory));
		if (curEnd == std::string::npos)
			return dirPos;
		curStart = curEnd + 1;
	}

	return nullptr;
}

void
DirectoryTree::lsDir(const LotusPath& internalPath) const
{
	const DirNode* printDir = getDirEntry(internalPath);
	if (printDir == nullptr)
		throw std::runtime_error("Directory was not found");

	size_t totalSize = 0;
	for (uint32_t x = 0; x < printDir->getFileCount(); x++)
	{
		totalSize += printDir->getChildFile(x)->getCompLen();
	}

	for (uint32_t x = 0; x < printDir->getDirCount(); x++)
	{
		const DirNode* curDir = printDir->getChildDir(x);
		std::cout << std::setw(8) << std::left << 4 << " "
			<< std::setw(12) << 0 << " "
			<< curDir->getName() << std::endl;
	}

	char buffer[80];
	for (uint32_t x = 0; x < printDir->getFileCount(); x++)
	{
		const FileNode* curFile = printDir->getChildFile(x);
		time_t epochTime = curFile->getTimeStamp() / 10000000UL - 11644473600UL;
		const tm* timeInfo = gmtime(&std::max((time_t)0, epochTime));
		strftime(buffer, 80, "%h %d %G", timeInfo);
		std::cout << std::setw(8) << std::left << curFile->getCompLen() << " "
			<< std::setw(12) << buffer << " "
			<< curFile->getName() << std::endl;
	}
}
