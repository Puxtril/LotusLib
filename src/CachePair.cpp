#include "CachePair.h"

using namespace LotusLib;

CachePair::CachePair(std::filesystem::path tocPath, std::filesystem::path cachePath, bool isPostEnsmallening)
	: m_isPostEnsmallening(isPostEnsmallening),
	m_tocPath(tocPath),
	m_cachePath(cachePath),
	m_dirTree(tocPath),
	m_hasReadToc(false)
{
}

void
CachePair::readToc()
{
	if (m_hasReadToc)
		return;

	logInfo(spdlog::fmt_lib::format("Reading {}", m_tocPath.filename().string()));

	std::ifstream tocReader(m_tocPath, std::ios_base::in | std::ios_base::binary);
	if (!isValid(tocReader))
		throw InvalidDataException("CachePair is not valid");
	tocReader.close();

	m_dirTree.readTocFile();
	m_hasReadToc = true;
}

void
CachePair::unReadToc()
{
	m_dirTree.unReadTocFile();
	m_hasReadToc = false;
}

bool
CachePair::isPostEnsmallening() const
{
	return m_isPostEnsmallening;
}

const std::filesystem::path&
CachePair::getTocPath() const
{
	return m_tocPath;
}

const std::filesystem::path&
CachePair::getCachePath() const
{
	return m_cachePath;
}

LotusLib::DirectoryTree::ConstFileIterator
CachePair::begin() const
{
	return const_cast<const DirectoryTree&>(m_dirTree).begin();
}

LotusLib::DirectoryTree::ConstFileIterator
CachePair::end() const
{
	return const_cast<const DirectoryTree&>(m_dirTree).end();
}

LotusLib::DirectoryTree::FileIteratorTree
CachePair::getIter(const LotusPath& path) const
{
	return const_cast<const DirectoryTree&>(m_dirTree).getIter(path);
}

LotusLib::DirectoryTree::FileIteratorTree
CachePair::getIter() const
{
	return m_dirTree.getIter();
}

bool
CachePair::isValid(std::ifstream& tocReader) const
{
	unsigned int magicNumber;
	unsigned int archiveVersion;

	tocReader.read(reinterpret_cast<char*>(&magicNumber), 4);
	tocReader.read(reinterpret_cast<char*>(&archiveVersion), 4);

	if (magicNumber != m_magicNumber)
	{
		logError(spdlog::fmt_lib::format("Invalid magic number: {}", magicNumber));
		return false;
	}
	if (archiveVersion != m_archiveVersion)
	{
		logError(spdlog::fmt_lib::format("Invalid archive version: {}", archiveVersion));
		return false;
	}

	return true;
}

FileEntries::DirNode*
CachePair::getDirEntry(const LotusPath& lotusPath)
{
	return m_dirTree.getDirEntry(lotusPath);
}

FileEntries::FileNode*
CachePair::getFileEntry(const LotusPath& lotusPath)
{
	return m_dirTree.getFileEntry(lotusPath);
}

const FileEntries::DirNode*
CachePair::getDirEntry(const LotusPath& lotusPath) const
{
	return m_dirTree.getDirEntry(lotusPath);
}

const FileEntries::FileNode*
CachePair::getFileEntry(const LotusPath& lotusPath) const
{
	return m_dirTree.getFileEntry(lotusPath);
}

size_t
CachePair::dirCount() const
{
	return m_dirTree.dirCount();
}

size_t
CachePair::fileCount() const
{
	return m_dirTree.fileCount();
}

size_t
CachePair::fileDupeCount() const
{
	return m_dirTree.fileDupeCount();
}

void
CachePair::lsDir(const LotusPath& internalPath) const
{
	m_dirTree.lsDir(internalPath);
}

std::vector<uint8_t>
CachePair::getData(const LotusPath& internalPath) const
{
	const FileEntries::FileNode* entry = m_dirTree.getFileEntry(internalPath);
	return getData(entry);
}

std::vector<uint8_t>
CachePair::getData(const FileEntries::FileNode* entry) const
{
	std::ifstream cacheReader(m_cachePath, std::ios::in | std::ios::binary);
	cacheReader.seekg(entry->getOffset(), std::ios_base::beg);
	std::vector<uint8_t> data(entry->getCompLen());
	cacheReader.read((char*)data.data(), entry->getCompLen());
	return data;
}

std::vector<uint8_t>
CachePair::getDataAndDecompress(const LotusPath& internalPath) const
{
	const FileNode* entry = m_dirTree.getFileEntry(internalPath);
	return getDataAndDecompress(entry);
}

std::vector<uint8_t>
CachePair::getDataAndDecompress(const FileEntries::FileNode* entry) const
{
	if (entry->getCompLen() == entry->getLen())
		return getData(entry);

	std::ifstream cacheReader(m_cachePath, std::ios::in | std::ios::binary);
	if (m_isPostEnsmallening)
		return Compression::getDataAndDecompressPost(entry, cacheReader);
	else
		return Compression::getDataAndDecompressPre(entry, cacheReader);
}