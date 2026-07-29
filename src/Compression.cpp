#include "LotusLib/Compression.h"

using namespace LotusLib::Impl;

std::vector<uint8_t>
Compression::decompressWarframePost(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader)
{
	std::vector<uint8_t> decompressedData(entry.len);
	decompressWarframePost(scratch, entry, cacheReader, decompressedData.data());
	return decompressedData;
}

void
Compression::decompressWarframePost(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader, uint8_t* outData)
{
	if (entry.compLen > static_cast<int32_t>(scratch->buf.size()))
		scratch->buf.resize(entry.compLen);

	int32_t decompPos = 0;
	int32_t compPos = 0;
	cacheReader.seekg(entry.cacheOffset, std::ios_base::beg);
	cacheReader.read((char*)scratch->buf.data(), entry.compLen);
	
	while (decompPos < entry.len)
	{
		std::tuple<uint32_t, uint32_t> blockLens = getWarframeBlockLens(&scratch->buf[compPos]);
		compPos += 8;

		if (std::get<0>(blockLens) == 0 && std::get<1>(blockLens) == 0)
		{
			compPos -= 8;
			blockLens = { entry.compLen, entry.len };
		}

		if (decompPos + (int32_t)std::get<1>(blockLens) > entry.len)
		{
			throw DecompressionException("Decompressed past the file length");
		}

		if (std::get<0>(blockLens) > std::min((size_t)getFileLen(cacheReader), (size_t)0x40000))
		{
			throw DecompressionException("Tried to read beyond limits, probably not a compressed file");
		}

		if (scratch->buf[compPos] == 0x8C)
			Compression::decompressOodle(&scratch->buf[compPos], std::get<0>(blockLens), &outData[decompPos], std::get<1>(blockLens));
		else
			Compression::decompressLz(&scratch->buf[compPos], std::get<0>(blockLens), &outData[decompPos], std::get<1>(blockLens));

		compPos += std::get<0>(blockLens);
		decompPos += std::get<1>(blockLens);
	}
}

std::vector<uint8_t>
Compression::decompressWarframePre(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader)
{
	std::vector<uint8_t> data(entry.len);
	decompressWarframePre(scratch, entry, cacheReader, data.data());
	return data;
}

void
Compression::decompressWarframePre(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader, uint8_t* outData)
{
	if (entry.compLen > static_cast<int32_t>(scratch->buf.size()))
		scratch->buf.resize(entry.compLen);

	int32_t decompPos = 0;
	int32_t compPos = 0;
	cacheReader.seekg(entry.cacheOffset, std::ios_base::beg);
	cacheReader.read((char*)scratch->buf.data(), entry.compLen);

	while (decompPos < entry.len)
	{
		std::tuple<uint32_t, uint32_t> blockLens = getEEBlockLensLz(&scratch->buf[compPos]);
		compPos += 4;

		Compression::decompressLz(&scratch->buf[compPos], std::get<0>(blockLens), &outData[decompPos], std::get<1>(blockLens));

		compPos += std::get<0>(blockLens);
		decompPos += std::get<1>(blockLens);
	}
}

std::vector<uint8_t>
Compression::decompressEE(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader)
{
	std::vector<uint8_t> decompressedData(entry.len);
	decompressEE(scratch, entry, cacheReader, decompressedData.data());
	return decompressedData;
}

void
Compression::decompressEE(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader, uint8_t* outData)
{
	if (entry.compLen > static_cast<int32_t>(scratch->buf.size()))
		scratch->buf.resize(entry.compLen);

	int32_t decompPos = 0;
	int32_t compPos = 0;
	cacheReader.seekg(entry.cacheOffset, std::ios_base::beg);
	cacheReader.read((char*)scratch->buf.data(), entry.compLen);
	
	while (decompPos < entry.len)
	{
		bool isOodleBlock = scratch->buf[compPos] > 0x7F;

		// Get block lengths
		std::tuple<uint32_t, uint32_t> blockLens;
		if (isOodleBlock)
		{
			blockLens = getEEBlockLensOodle(&scratch->buf[compPos]);
			compPos += 8;
		}
		else
		{
			blockLens = getEEBlockLensLz(&scratch->buf[compPos]);
			compPos += 4;
		}
		
		// Sanity checks
		if (std::get<0>(blockLens) == 0 && std::get<1>(blockLens) == 0)
			blockLens = { entry.compLen, entry.len };

		if (decompPos + (int32_t)std::get<1>(blockLens) > entry.len)
		{
			throw DecompressionException("Decompressed past the file length");
		}

		if (std::get<0>(blockLens) > std::min((size_t)getFileLen(cacheReader), (size_t)0x40000))
		{
			throw DecompressionException("Tried to read beyond limits, probably not a compressed file");
		}

		// Decompress
		if (isOodleBlock)
			Compression::decompressOodle(&scratch->buf[compPos], std::get<0>(blockLens), &outData[decompPos], std::get<1>(blockLens));
		else
			Compression::decompressLz(&scratch->buf[compPos], std::get<0>(blockLens), &outData[decompPos], std::get<1>(blockLens));

		compPos += std::get<0>(blockLens);
		decompPos += std::get<1>(blockLens);
	}
}

std::vector<uint8_t>
Compression::decompressKeystone(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader)
{
	std::vector<uint8_t> decompressedData(entry.len);
	decompressKeystone(scratch, entry, cacheReader, decompressedData.data());
	return decompressedData;
}

void
Compression::decompressKeystone(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader, uint8_t* outData)
{
	if (entry.compLen > static_cast<int32_t>(scratch->buf.size()))
		scratch->buf.resize(entry.compLen);

	int32_t decompPos = 0;
	int32_t compPos = 0;
	cacheReader.seekg(entry.cacheOffset, std::ios_base::beg);
	cacheReader.read((char*)scratch->buf.data(), entry.compLen);
	
	while (decompPos < entry.len)
	{
		// Get block lengths
		std::tuple<uint32_t, uint32_t> blockLens = getKeystoneBlockLens(&scratch->buf[compPos]);
		compPos += 4;
		
		// Such a STUPID edge case where the compressed length is invalid
		if (std::get<0>(blockLens) > 16384)
		{
			// Last block, we can calculate the compressed length
			if (decompPos + std::get<1>(blockLens) == entry.len)
			{
				blockLens = {entry.compLen - compPos, std::get<1>(blockLens)};
			}
			// Not the last block. Compressed size is just 1...
			else
			{
				if (scratch->buf[compPos+5] == 2)
					blockLens = {5, std::get<1>(blockLens)};
				else
					blockLens = {6, std::get<1>(blockLens)};
			}
		}

		if (decompPos + (int32_t)std::get<1>(blockLens) > entry.len)
			throw DecompressionException("Decompressed past the file length");

		if (std::get<0>(blockLens) + compPos > entry.compLen)
			throw DecompressionException("Tried to read beyond limits");

		// Decompress
		if (std::get<0>(blockLens) < std::get<1>(blockLens))
			Compression::decompressOodle(&scratch->buf[compPos], std::get<0>(blockLens), &outData[decompPos], std::get<1>(blockLens));
		else
		 	std::memcpy(&outData[decompPos], &scratch->buf[compPos], std::get<0>(blockLens));

		compPos += std::get<0>(blockLens);
		decompPos += std::get<1>(blockLens);
	}
}

void
Compression::decompressDarkSector(CompressionScratch* scratch, const FileNode& entry, std::ifstream& cacheReader, uint8_t* outData)
{
	if (entry.compLen > static_cast<int32_t>(scratch->buf.size()))
		scratch->buf.resize(entry.compLen);
	
	cacheReader.seekg(entry.cacheOffset + 26, std::ios::beg);
	uint16_t nameLen;
	cacheReader.read((char*)&nameLen, 2);
	uint16_t extraLen;
	cacheReader.read((char*)&extraLen, 2);
	cacheReader.seekg(nameLen + extraLen, std::ios::cur);

	int32_t decompPos = 0;
	int32_t compPos = 0;
	cacheReader.read((char*)scratch->buf.data(), entry.compLen);

	while (decompPos < entry.len)
	{
		// Get block lengths
		std::tuple<uint32_t, uint32_t> blockLens = getDarkSectorBlockLens(&scratch->buf[compPos]);
		compPos += 8;
		
		// Sanity checks
		if (std::get<0>(blockLens) == 0 && std::get<1>(blockLens) == 0)
			blockLens = { entry.compLen, entry.len };

		if (decompPos + (int32_t)std::get<1>(blockLens) > entry.len)
			throw DecompressionException("Decompressed past the file length");

		if (std::get<0>(blockLens) > std::min((size_t)getFileLen(cacheReader), (size_t)0x40000))
			throw DecompressionException("Tried to read beyond limits, probably not a compressed file");

		unsigned int decompressedLen = Compression::decompressLz(&scratch->buf[compPos], std::get<0>(blockLens), &outData[decompPos], std::get<1>(blockLens));
		if (decompressedLen != std::get<1>(blockLens))
			throw DecompressionException("Did not decompress expected data length: " + std::to_string(decompressedLen) + ", expected " + std::to_string(std::get<1>(blockLens)));

		compPos += std::get<0>(blockLens);
		decompPos += std::get<1>(blockLens);
	}
}

std::tuple<uint32_t, uint32_t>
Compression::getWarframeBlockLens(uint8_t* data)
{
	if (data[0] != 0x80 || (data[7] & 0x0F) != 0x1)
	{
		return { 0, 0 };
	}

	// Read 2 `uint32_t`s as big endian
	uint32_t num1 = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
	uint32_t num2 = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
	uint32_t blockComLen = (num1 >> 2) & 0xFFFFFF;
	uint32_t blockDecomLen = (num2 >> 5) & 0xFFFFFF;

	return { blockComLen, blockDecomLen };
}

std::tuple<uint32_t, uint32_t>
Compression::getEEBlockLensOodle(uint8_t* data)
{
	// Read 2 `uint32_t`s as big endian
	uint32_t num1 = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
	uint32_t num2 = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
	uint32_t blockComLen = (num1 >> 2) & 0xFFFFFF;
	uint32_t blockDecomLen = (num2 >> 5) & 0xFFFFFF;

	return { blockComLen, blockDecomLen };
}

std::tuple<uint16_t, uint16_t>
Compression::getEEBlockLensLz(uint8_t* data)
{
	// Read 2 `uint16_t`s as big endian
	uint16_t num1 = (data[0] << 8) | data[1];
	uint16_t num2 = (data[2] << 8) | data[3];

	return { num1, num2 };
}

std::tuple<uint32_t, uint32_t>
Compression::getKeystoneBlockLens(uint8_t* data)
{
	uint32_t blockLen = 0;
	uint32_t blockCompLen = 0;
	
	blockLen = (data[2] << 8) | data[3];

	// Uncompressed data
	if (data[0] == data[2] && data[1] == data[3])
	{
		blockCompLen = blockLen;
	}
	else if (data[5] == 2)
	{
		blockCompLen = (data[6] << 8) | data[7];
		blockCompLen += 5;
	}
	else
	{
		blockCompLen = ((data[7] << 8) | data[8]);
		blockCompLen += 6;
	}

	return { blockCompLen, blockLen };
}

std::tuple<uint32_t, uint32_t>
Compression::getDarkSectorBlockLens(uint8_t* data)
{
	uint32_t blockComLen = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
	uint32_t blockDecomLen = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];

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

unsigned int
Compression::decompressLz(uint8_t* inputData, uint32_t inputLen, uint8_t* outputData, uint32_t outputLen)
{
	if (inputLen == outputLen)
	{
		outputData = inputData;
		return inputLen;
	}
	else
	{
		return lzf_decompress((char*)inputData, inputLen, (char*)outputData, outputLen);
	}
}
