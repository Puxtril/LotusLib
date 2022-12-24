#include "CachePairReader.h"

using namespace LotusLib;

CachePairReader::CachePairReader(std::filesystem::path tocPath, std::filesystem::path cachePath, bool isPostEnsmallening)
	: CachePair(tocPath, cachePath, isPostEnsmallening)
{
}

int
CachePairReader::getData(const LotusPath& internalPath, char*& data) const
{
	const FileEntries::FileNode* entry = m_dirTree.getFileEntry(internalPath);
	data = this->getData(entry);
	return entry->getLen();
}

char*
CachePairReader::getData(const FileEntries::FileNode* entry) const
{
	std::ifstream cacheReader(m_cachePath, std::ios::in | std::ios::binary);
	cacheReader.seekg(entry->getOffset(), std::ios_base::beg);
	char* data = new char[entry->getLen()];
	cacheReader.read(data, entry->getCompLen());
	return data;
}

int
CachePairReader::getDataAndDecompress(const LotusPath& internalPath, char*& outData) const
{
	const FileNode* entry = m_dirTree.getFileEntry(internalPath);
	outData = getDataAndDecompress(entry);
	return entry->getLen();
}

char*
CachePairReader::getDataAndDecompress(const FileEntries::FileNode* entry) const
{
	if (entry->getCompLen() == entry->getLen())
		return getData(entry);

	std::ifstream cacheReader(m_cachePath, std::ios::in | std::ios::binary);
	if (m_isPostEnsmallening)
		return CompressionLotus::getDataAndDecompressPost(entry, cacheReader);
	else
		return CompressionLotus::getDataAndDecompressPre(entry, cacheReader);
}
