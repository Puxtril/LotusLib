#include "CachePair.h"

using namespace LotusLib;

CachePair::CachePair(std::filesystem::path tocPath, std::filesystem::path cachePath, bool isPostEnsmallening)
	: m_isPostEnsmallening(isPostEnsmallening),
	m_tocPath(tocPath),
	m_cachePath(cachePath),
	m_dirTree(tocPath),
	m_hasReadToc(false),
	m_logger(Logger::getInstance())
{
}

void
CachePair::readToc()
{
	if (m_hasReadToc)
		return;

	m_logger.info(spdlog::fmt_lib::format("Reading {}", m_tocPath.filename().string()));

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

LotusLib::DirectoryTree::ConstFileIteratorTree
CachePair::getIter(const LotusPath& path) const
{
	return const_cast<const DirectoryTree&>(m_dirTree).getIter(path);
}

LotusLib::DirectoryTree::ConstFileIteratorTree
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
		m_logger.error(spdlog::fmt_lib::format("Invalid magic number: {} != {}", magicNumber, m_magicNumber));
		return false;
	}
	if (archiveVersion != m_archiveVersion)
	{
		m_logger.error(spdlog::fmt_lib::format("Invalid archive version: {} != {}", archiveVersion, m_archiveVersion));
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

void
CachePair::lsDir(const LotusPath& internalPath) const
{
	m_dirTree.lsDir(internalPath);
}
