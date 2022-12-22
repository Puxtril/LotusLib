#pragma once

#include <iostream>

#include "LotusExceptions.h"
#include "Logger.h"
#include "oodle/oodle2.h"

namespace LotusLib {
	//! \brief Decompression algorithms
	class Compression
	{
	public:
		//! \brief Decompresses a block of oodle data.
		//!
		//! This will not check if the data is valid oodle or not, and will not throw any exception.
		//! 
		//! \param inputData The oodle-compressed data
		//! \param inputLen Length of the input data
		//! \param outputData Destination for the oodle-decompressed data
		//! \param outputLen Length of the output data array
		static void decompressOodle(char* inputData, size_t inputLen, char* outputData, size_t outputLen);

		//! \brief Decompresses a block of LZ data
		//!
		//! This doesn't fully conform to LZ decompressors
		//! Will throw exceptions if bad data is encountered
		//! Code was taken from GMMan's Evolution Engine Cache Extractor
		//! https://forum.xentax.com/viewtopic.php?t=10782
		//!
		//! \param inputData The LZ-compressed data
		//! \param inputLen Length of the input data
		//! \param outputData Destination for the LZ-decompressed data
		//! \param outputLen Length of the output data array
		static void decompressLz(char* inputData, uint32_t inputLen, char* outputData, uint32_t outputLen);

	private:
		static void decompressLzBlock(const unsigned char* inputData, uint32_t inputLen, unsigned char* outputData, uint32_t outputLen);
		static void decompressLzBlockHelper(const unsigned char* compressedData, int32_t compressedDataLen, unsigned char* decompressedData, int32_t decompressedDataLen);
	};
}
