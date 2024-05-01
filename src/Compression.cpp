#include "Compression.h"

using namespace LotusLib;

std::vector<uint8_t>
Compression::getDataAndDecompressPost(const FileEntries::FileNode* entry, std::ifstream& cacheReader)
{
	std::vector<uint8_t> decompressedData(entry->getLen());
	getDataAndDecompressPost(entry, cacheReader, decompressedData.data());
	return decompressedData;
}

void
Compression::getDataAndDecompressPost(const FileEntries::FileNode* entry, std::ifstream& cacheReader, uint8_t* outData)
{
	static uint8_t* compressedBuffer = new uint8_t[0x40000];
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
		cacheReader.read((char*)compressedBuffer, std::get<0>(blockLens));

		if (isOodle)
		{
			Compression::decompressOodle(compressedBuffer, std::get<0>(blockLens), &outData[decompPos], std::get<1>(blockLens));
			oodleBlockCount++;
		}
		else
		{
			Compression::decompressLz(compressedBuffer, std::get<0>(blockLens), &outData[decompPos], std::get<1>(blockLens));
			lzBlockCount++;
		}
		decompPos += std::get<1>(blockLens);
	}
}

std::vector<uint8_t>
Compression::getDataAndDecompressPre(const FileEntries::FileNode* entry, std::ifstream& cacheReader)
{
	std::vector<uint8_t> data(entry->getLen());
	getDataAndDecompressPre(entry, cacheReader, data.data());
	return data;
}

void
Compression::getDataAndDecompressPre(const FileEntries::FileNode* entry, std::ifstream& cacheReader, uint8_t* outData)
{
	std::vector<uint8_t> compressedData(entry->getCompLen());
	cacheReader.seekg(entry->getOffset(), std::ios_base::beg);
	cacheReader.read((char*)compressedData.data(), entry->getCompLen());

	Compression::decompressLz(compressedData.data(), entry->getCompLen(), outData, entry->getLen());
}

bool
Compression::isOodleBlock(std::ifstream& cacheReader)
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
Compression::getBlockLens(std::ifstream& cacheReader)
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
		Compression::decompressLzBlock(reinterpret_cast<unsigned char*>(inputData), inputLen, reinterpret_cast<unsigned char*>(outputData), outputLen);

}

void
Compression::decompressLzBlock(const uint8_t* inputData, uint32_t inputLen, uint8_t* outputData, uint32_t outputLen)
{
	uint32_t inputPos = 0;
	uint32_t outputPos = 0;

	while (outputPos < outputLen)
	{
		// Get block lengths, little endian
		int blockLen = (inputData[inputPos] << 8) | inputData[inputPos + 1];
		int decompLen = (inputData[inputPos + 2] << 8) | inputData[inputPos + 3];
		char firstFour[4];
		memcpy(&firstFour[0], inputData, 4);
		inputPos += 4;

		if (blockLen != decompLen)
		{
			Compression::decompressLzBlockHelper((unsigned char*)&(inputData[inputPos]), blockLen, &outputData[outputPos], decompLen);
		}
		else
		{
			memcpy(outputData + outputPos, inputData + inputPos, blockLen);
		}
		outputPos += decompLen;
		inputPos += blockLen;
	}
	if (outputPos != outputLen) throw DecompressionException("Decompressed length does not match length in file entry.");
}

void
Compression::decompressLzBlockHelper(const unsigned char* compressedData, int32_t compressedDataLen, unsigned char* decompressedData, int32_t decompressedDataLen)
{
	uint32_t compPos = 0;
	uint32_t decompPos = 0;

	while (compPos < compressedDataLen)
	{
		uint32_t codeWord = compressedData[compPos++];
		if (codeWord <= 0x1f)
		{
			// Encode literal
			if (decompPos + codeWord + 1 > decompressedDataLen) throw DecompressionException("Attempting to index past decompression buffer.");
			if (compPos + codeWord + 1 > compressedDataLen) throw DecompressionException("Attempting to index past compression buffer.");
			memcpy(decompressedData + decompPos, compressedData + compPos, codeWord + 1);
			decompPos += codeWord + 1;
			compPos += codeWord + 1;
		}
		else
		{
			// Encode dictionary
			int copyLen = codeWord >> 5; // High 3 bits are copy length
			if (copyLen == 7) // If those three make 7, then there are more bytes to copy (maybe)
			{
				if (compPos >= compressedDataLen) throw DecompressionException("Attempting to index past compression buffer.");
				copyLen += compressedData[compPos++]; // Grab next byte and add 7 to it
			}
			if (compPos >= compressedDataLen) throw DecompressionException("Attempting to index past compression buffer.");
			int dictDist = ((codeWord & 0x1f) << 8) | compressedData[compPos++]; // 13 bits code lookback offset
			copyLen += 2; // Add 2 to copy length
			if (decompPos + copyLen > decompressedDataLen) throw DecompressionException("Attempting to index past decompression buffer.");
			int decompDistBeginPos = decompPos - 1 - dictDist;
			if (decompDistBeginPos < 0) throw DecompressionException("Attempting to index below decompression buffer.");

			for (int i = 0; i < copyLen; ++i, ++decompPos)
			{
				decompressedData[decompPos] = decompressedData[decompDistBeginPos + i];
			}
		}
	}
	if (decompPos != decompressedDataLen) throw DecompressionException("Decoder did not decode all bytes.");
}
