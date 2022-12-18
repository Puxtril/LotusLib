#include "CompressionLotus.h"

using namespace LotusLib;

char*
CompressionLotus::getDataAndDecompressPost(const FileEntries::FileNode* entry, std::ifstream& cacheReader)
{
	static char* compressedBuffer = new char[0x40000];
	char* decompressedData = new char[entry->getLen()];
	int32_t decompPos = 0;
	cacheReader.seekg(entry->getOffset(), std::ios_base::beg);
	
	int oodleBlockCount = 0;
	int lzBlockCount = 0;
	while (decompPos < entry->getLen())
	{
		std::tuple<uint32_t, uint32_t> blockLens = getBlockLens(cacheReader);

		if (std::get<0>(blockLens) == 0 && std::get<1>(blockLens) == 0)
			blockLens = { entry->getCompLen(), entry->getLen() };

		if (decompPos + std::get<1>(blockLens) > entry->getLen())
		{
			Logger::getInstance().error("Decompressed past the file length");
			throw DecompressionException("Decompressed past the file length");
		}

		if (std::get<0>(blockLens) > std::min((size_t)getFileLen(cacheReader), (size_t)0x40000))
		{
			Logger::getInstance().error("Tried to read beyond limits, probably not a compressed file");
			throw DecompressionException("Tried to read beyond limits, probably not a compressed file");
		}

		bool isOodle = isOodleBlock(cacheReader);
		cacheReader.read(compressedBuffer, std::get<0>(blockLens));

		if (isOodle)
		{
			Compression::decompressOodle(compressedBuffer, std::get<0>(blockLens), &decompressedData[decompPos], std::get<1>(blockLens));
			oodleBlockCount++;
		}
		else
		{
			Compression::decompressLz(compressedBuffer, std::get<0>(blockLens), &decompressedData[decompPos], std::get<1>(blockLens));
			lzBlockCount++;
		}
		decompPos += std::get<1>(blockLens);
	}

	return decompressedData;
}

char*
CompressionLotus::getDataAndDecompressPre(const FileEntries::FileNode* entry, std::ifstream& cacheReader)
{
	char* decompressedData = new char[entry->getLen()];

	char* compressedData = new char[entry->getCompLen()];
	cacheReader.seekg(entry->getOffset(), std::ios_base::beg);
	cacheReader.read(compressedData, entry->getCompLen());

	Compression::decompressLz(compressedData, entry->getCompLen(), decompressedData, entry->getLen());

	delete[] compressedData;
	return decompressedData;
}

bool
CompressionLotus::isOodleBlock(std::ifstream& cacheReader)
{
	unsigned char checkMagic;
	cacheReader.read(reinterpret_cast<char*>(&checkMagic), 1);
	cacheReader.seekg(-1, std::ios_base::cur);
	if (checkMagic == 0x8C)
	{
		return true;
	}
	return false;
}

std::tuple<uint32_t, uint32_t>
CompressionLotus::getBlockLens(std::ifstream& cacheReader)
{
	unsigned char blockInfo[8];
	cacheReader.read(reinterpret_cast<char*>(&blockInfo), 8);

	if (blockInfo[0] != 0x80 || (blockInfo[7] & 0x0F) != 0x1)
	{
		cacheReader.seekg(-8, std::ios_base::cur);
		return { 0, 0 };
	}

	uint32_t num1 = (blockInfo[0] << 24) | (blockInfo[1] << 16) | (blockInfo[2] << 8) | blockInfo[3];
	uint32_t num2 = (blockInfo[4] << 24) | (blockInfo[5] << 16) | (blockInfo[6] << 8) | blockInfo[7];
	uint32_t blockComLen = (num1 >> 2) & 0xFFFFFF;
	uint32_t blockDecomLen = (num2 >> 5) & 0xFFFFFF;

	return { blockComLen, blockDecomLen };
}

std::streampos
CompressionLotus::getFileLen(std::ifstream& file)
{
	std::streampos offset = file.tellg();
	file.seekg(0, std::ios::end);
	std::streampos size = file.tellg();
	file.seekg(offset, std::ios::beg);
	return size;
}
