#include "Compression.h"

using namespace LotusLib;

uint8_t Compression::m_compressedBuffer[0x40000];

std::vector<uint8_t>
Compression::decompressWarframePost(const FileEntries::FileNode* entry, std::ifstream& cacheReader)
{
	std::vector<uint8_t> decompressedData(entry->getLen());
	decompressWarframePost(entry, cacheReader, decompressedData.data());
	return decompressedData;
}

void
Compression::decompressWarframePost(const FileEntries::FileNode* entry, std::ifstream& cacheReader, uint8_t* outData)
{
	int32_t decompPos = 0;
	cacheReader.seekg(entry->getOffset(), std::ios_base::beg);
	
	while (decompPos < entry->getLen())
	{
		std::tuple<uint32_t, uint32_t> blockLens = getWarframeBlockLens(cacheReader);

		if (std::get<0>(blockLens) == 0 && std::get<1>(blockLens) == 0)
			blockLens = { entry->getCompLen(), entry->getLen() };

		if (decompPos + (int32_t)std::get<1>(blockLens) > entry->getLen())
		{
			logError("Decompressed past the file length");
			throw DecompressionException("Decompressed past the file length");
		}

		if (std::get<0>(blockLens) > std::min((size_t)getFileLen(cacheReader), (size_t)0x40000))
		{
			logError("Tried to read beyond limits, probably not a compressed file");
			throw DecompressionException("Tried to read beyond limits, probably not a compressed file");
		}

		bool isOodle = isWarframeOodleBlock(cacheReader);
		cacheReader.read((char*)m_compressedBuffer, std::get<0>(blockLens));

		if (isOodle)
			Compression::decompressOodle(m_compressedBuffer, std::get<0>(blockLens), &outData[decompPos], std::get<1>(blockLens));
		else
			Compression::decompressLz(m_compressedBuffer, std::get<0>(blockLens), &outData[decompPos], std::get<1>(blockLens));

		decompPos += std::get<1>(blockLens);
	}
}

std::vector<uint8_t>
Compression::decompressWarframePre(const FileEntries::FileNode* entry, std::ifstream& cacheReader)
{
	std::vector<uint8_t> data(entry->getLen());
	decompressWarframePre(entry, cacheReader, data.data());
	return data;
}

void
Compression::decompressWarframePre(const FileEntries::FileNode* entry, std::ifstream& cacheReader, uint8_t* outData)
{
	cacheReader.seekg(entry->getOffset(), std::ios_base::beg);
	cacheReader.read((char*)m_compressedBuffer, entry->getCompLen());
	Compression::decompressLz(m_compressedBuffer + 4, entry->getCompLen(), outData, entry->getLen());
}

std::vector<uint8_t>
Compression::decompressEE(const FileEntries::FileNode* entry, std::ifstream& cacheReader)
{
	std::vector<uint8_t> decompressedData(entry->getLen());
	decompressEE(entry, cacheReader, decompressedData.data());
	return decompressedData;
}

void
Compression::decompressEE(const FileEntries::FileNode* entry, std::ifstream& cacheReader, uint8_t* outData)
{
	int32_t decompPos = 0;
	cacheReader.seekg(entry->getOffset(), std::ios_base::beg);
	
	while (decompPos < entry->getLen())
	{
		bool isOodleBlock = isEEOodleBlock(cacheReader);

		// Get block lengths
		std::tuple<uint32_t, uint32_t> blockLens;
		if (isOodleBlock)
			blockLens = getEEBlockLensOodle(cacheReader);
		else
			blockLens = getEEBlockLensLz(cacheReader);
		
		// Sanity checks
		if (std::get<0>(blockLens) == 0 && std::get<1>(blockLens) == 0)
			blockLens = { entry->getCompLen(), entry->getLen() };

		if (decompPos + (int32_t)std::get<1>(blockLens) > entry->getLen())
		{
			logError("Decompressed past the file length");
			throw DecompressionException("Decompressed past the file length");
		}

		if (std::get<0>(blockLens) > std::min((size_t)getFileLen(cacheReader), (size_t)0x40000))
		{
			logError("Tried to read beyond limits, probably not a compressed file");
			throw DecompressionException("Tried to read beyond limits, probably not a compressed file");
		}

		// Decompress
		cacheReader.read((char*)m_compressedBuffer, std::get<0>(blockLens));

		if (isOodleBlock)
			Compression::decompressOodle(m_compressedBuffer, std::get<0>(blockLens), &outData[decompPos], std::get<1>(blockLens));
		else
			Compression::decompressLz(m_compressedBuffer, std::get<0>(blockLens), &outData[decompPos], std::get<1>(blockLens));

		decompPos += std::get<1>(blockLens);
	}
}

bool
Compression::isWarframeOodleBlock(std::ifstream& cacheReader)
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

bool
Compression::isEEOodleBlock(std::ifstream& cacheReader)
{
	unsigned char checkFirstNum;
	cacheReader.read(reinterpret_cast<char*>(&checkFirstNum), 1);
	cacheReader.seekg(-1, std::ios_base::cur);

	if (checkFirstNum > 0x7F)
		return true;
	return false;
}

std::tuple<uint32_t, uint32_t>
Compression::getWarframeBlockLens(std::ifstream& cacheReader)
{
	static unsigned char blockInfo[8];
	cacheReader.read(reinterpret_cast<char*>(&blockInfo), 8);

	if (blockInfo[0] != 0x80 || (blockInfo[7] & 0x0F) != 0x1)
	{
		cacheReader.seekg(-8, std::ios_base::cur);
		return { 0, 0 };
	}

	// Read 2 `uint32_t`s as big endian
	uint32_t num1 = (blockInfo[0] << 24) | (blockInfo[1] << 16) | (blockInfo[2] << 8) | blockInfo[3];
	uint32_t num2 = (blockInfo[4] << 24) | (blockInfo[5] << 16) | (blockInfo[6] << 8) | blockInfo[7];
	uint32_t blockComLen = (num1 >> 2) & 0xFFFFFF;
	uint32_t blockDecomLen = (num2 >> 5) & 0xFFFFFF;

	return { blockComLen, blockDecomLen };
}

std::tuple<uint16_t, uint16_t>
Compression::getEEBlockLensLz(std::ifstream& cacheReader)
{
	static unsigned char blockInfo[4];
	cacheReader.read(reinterpret_cast<char*>(&blockInfo), 4);

	// Read 2 `uint16_t`s as big endian
	uint16_t num1 = (blockInfo[0] << 8) | blockInfo[1];
	uint16_t num2 = (blockInfo[2] << 8) | blockInfo[3];

	return { num1, num2 };
}

std::tuple<uint32_t, uint32_t>
Compression::getEEBlockLensOodle(std::ifstream& cacheReader)
{
	static unsigned char blockInfo[8];
	cacheReader.read(reinterpret_cast<char*>(&blockInfo), 8);

	// Read 2 `uint32_t`s as big endian
	uint32_t num1 = (blockInfo[0] << 24) | (blockInfo[1] << 16) | (blockInfo[2] << 8) | blockInfo[3];
	uint32_t num2 = (blockInfo[4] << 24) | (blockInfo[5] << 16) | (blockInfo[6] << 8) | blockInfo[7];
	uint32_t blockComLen = (num1 >> 2) & 0xFFFFFF;
	uint32_t blockDecomLen = (num2 >> 5) & 0xFFFFFF;

	return { blockComLen, blockDecomLen };
}

std::streampos
Compression::getFileLen(std::ifstream& file)
{
	std::streampos offset = file.tellg();
	file.seekg(0, std::ios::end);
	std::streampos size = file.tellg();
	file.seekg(offset, std::ios::beg);
	return size;
}

void
Compression::decompressOodle(uint8_t* inputData, size_t inputLen, uint8_t* outputData, size_t outputLen)
{
	OodleLZ_Decompress(inputData, inputLen, outputData, outputLen, OodleLZ_FuzzSafe_No, OodleLZ_CheckCRC_No, OodleLZ_Verbosity_None, 0, 0, 0, 0, 0, 0, OodleLZ_Decode_ThreadPhaseAll);
}

void
Compression::decompressLz(uint8_t* inputData, uint32_t inputLen, uint8_t* outputData, uint32_t outputLen)
{
	if (inputLen == outputLen)
		outputData = inputData;
	else
		lzf_decompress((char*)inputData, inputLen, (char*)outputData, outputLen);

}
