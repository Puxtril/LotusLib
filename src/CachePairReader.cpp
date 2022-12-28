#include "CachePairReader.h"

using namespace LotusLib;

CachePairReader::CachePairReader(std::filesystem::path tocPath, std::filesystem::path cachePath, bool isPostEnsmallening)
	: CachePair(tocPath, cachePath, isPostEnsmallening)
{
}

int
CachePairReader::getData(const LotusPath& internalPath, char* outData) const
{
	const FileEntries::FileNode* entry = m_dirTree.getFileEntry(internalPath);
	std::ifstream cacheReader(m_cachePath, std::ios::in | std::ios::binary);
	cacheReader.seekg(entry->getOffset(), std::ios_base::beg);
	cacheReader.read(outData, entry->getCompLen());
	return entry->getLen();
}

std::unique_ptr<char[]>
CachePairReader::getData(const FileEntries::FileNode* entry) const
{
	std::ifstream cacheReader(m_cachePath, std::ios::in | std::ios::binary);
	cacheReader.seekg(entry->getOffset(), std::ios_base::beg);
	std::unique_ptr<char[]> data(new char[entry->getLen()]);
	cacheReader.read(&data[0], entry->getCompLen());
	return data;
}

int
CachePairReader::getDataAndDecompress(const LotusPath& internalPath, char* outData) const
{
	const FileNode* entry = m_dirTree.getFileEntry(internalPath);
	
	if (entry->getCompLen() == entry->getLen())
		return getData(internalPath, outData);

	std::ifstream cacheReader(m_cachePath, std::ios::in | std::ios::binary);
	if (m_isPostEnsmallening)
		CompressionLotus::getDataAndDecompressPost(entry, cacheReader, outData);
	else
		CompressionLotus::getDataAndDecompressPre(entry, cacheReader, outData);
	
	return entry->getLen();
}

std::unique_ptr<char[]>
CachePairReader::getDataAndDecompress(const FileEntries::FileNode* entry) const
{
	if (entry->getCompLen() == entry->getLen())
		return getData(entry);

	char* rawData;
	std::ifstream cacheReader(m_cachePath, std::ios::in | std::ios::binary);
	if (m_isPostEnsmallening)
		rawData = CompressionLotus::getDataAndDecompressPost(entry, cacheReader);
	else
		rawData = CompressionLotus::getDataAndDecompressPre(entry, cacheReader);

	std::unique_ptr<char[]> data(rawData);
	return data;
}
