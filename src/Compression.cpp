#include "Compression.h"

void
LotusLib::Compression::decompressOodle(uint8_t* inputData, size_t inputLen, uint8_t* outputData, size_t outputLen)
{
	OodleLZ_Decompress(inputData, inputLen, outputData, outputLen, OodleLZ_FuzzSafe_No, OodleLZ_CheckCRC_No, OodleLZ_Verbosity_None, 0, 0, 0, 0, 0, 0, OodleLZ_Decode_ThreadPhaseAll);
}

void
LotusLib::Compression::decompressLz(uint8_t* inputData, uint32_t inputLen, uint8_t* outputData, uint32_t outputLen)
{
	if (inputLen == outputLen)
		outputData = inputData;
	else
		LotusLib::Compression::decompressLzBlock(reinterpret_cast<unsigned char*>(inputData), inputLen, reinterpret_cast<unsigned char*>(outputData), outputLen);

}

void
LotusLib::Compression::decompressLzBlock(const uint8_t* inputData, uint32_t inputLen, uint8_t* outputData, uint32_t outputLen)
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
			LotusLib::Compression::decompressLzBlockHelper((unsigned char*)&(inputData[inputPos]), blockLen, &outputData[outputPos], decompLen);
		}
		else
		{
			memcpy(outputData + outputPos, inputData + inputPos, blockLen);
		}
		outputPos += decompLen;
		inputPos += blockLen;
	}
	if (outputPos != outputLen) throw LotusLib::DecompressionException("Decompressed length does not match length in file entry.");
}

void
LotusLib::Compression::decompressLzBlockHelper(const unsigned char* compressedData, int32_t compressedDataLen, unsigned char* decompressedData, int32_t decompressedDataLen)
{
	uint32_t compPos = 0;
	uint32_t decompPos = 0;

	while (compPos < compressedDataLen)
	{
		uint32_t codeWord = compressedData[compPos++];
		if (codeWord <= 0x1f)
		{
			// Encode literal
			if (decompPos + codeWord + 1 > decompressedDataLen) throw LotusLib::DecompressionException("Attempting to index past decompression buffer.");
			if (compPos + codeWord + 1 > compressedDataLen) throw LotusLib::DecompressionException("Attempting to index past compression buffer.");
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
				if (compPos >= compressedDataLen) throw LotusLib::DecompressionException("Attempting to index past compression buffer.");
				copyLen += compressedData[compPos++]; // Grab next byte and add 7 to it
			}
			if (compPos >= compressedDataLen) throw LotusLib::DecompressionException("Attempting to index past compression buffer.");
			int dictDist = ((codeWord & 0x1f) << 8) | compressedData[compPos++]; // 13 bits code lookback offset
			copyLen += 2; // Add 2 to copy length
			if (decompPos + copyLen > decompressedDataLen) throw LotusLib::DecompressionException("Attempting to index past decompression buffer.");
			int decompDistBeginPos = decompPos - 1 - dictDist;
			if (decompDistBeginPos < 0) throw LotusLib::DecompressionException("Attempting to index below decompression buffer.");

			for (int i = 0; i < copyLen; ++i, ++decompPos)
			{
				decompressedData[decompPos] = decompressedData[decompDistBeginPos + i];
			}
		}
	}
	if (decompPos != decompressedDataLen) throw LotusLib::DecompressionException("Decoder did not decode all bytes.");
}
