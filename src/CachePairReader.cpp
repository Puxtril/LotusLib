#include "CachePairReader.h"

using namespace LotusLib;

CachePairReader::CachePairReader(std::filesystem::path tocPath, std::filesystem::path cachePath, bool isPostEnsmallening)
	: CachePair(tocPath, cachePath, isPostEnsmallening)
{
}

std::vector<uint8_t>
CachePairReader::getData(const LotusPath& internalPath) const
{
	const FileEntries::FileNode* entry = m_dirTree.getFileEntry(internalPath);
	return getData(entry);
}

std::vector<uint8_t>
CachePairReader::getData(const FileEntries::FileNode* entry) const
{
	std::ifstream cacheReader(m_cachePath, std::ios::in | std::ios::binary);
	cacheReader.seekg(entry->getOffset(), std::ios_base::beg);
	std::vector<uint8_t> data(entry->getCompLen());
	cacheReader.read((char*)data.data(), entry->getCompLen());
	return data;
}

std::vector<uint8_t>
CachePairReader::getDataAndDecompress(const LotusPath& internalPath) const
{
	const FileNode* entry = m_dirTree.getFileEntry(internalPath);
	return getDataAndDecompress(entry);
}

std::vector<uint8_t>
CachePairReader::getDataAndDecompress(const FileEntries::FileNode* entry) const
{
	if (entry->getCompLen() == entry->getLen())
		return getData(entry);

	std::ifstream cacheReader(m_cachePath, std::ios::in | std::ios::binary);
	if (m_isPostEnsmallening)
		return Compression::getDataAndDecompressPost(entry, cacheReader);
	else
		return Compression::getDataAndDecompressPre(entry, cacheReader);
}
