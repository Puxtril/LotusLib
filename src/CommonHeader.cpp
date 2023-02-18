#include "CommonHeader.h"

using namespace LotusLib;
		
int
CommonHeaderReader::findHeaderLen(const char* file)
{
	size_t filePos = 0;

	uint32_t sourcePathCount = *reinterpret_cast<const uint32_t*>(&file[filePos]);
	filePos += 4;
	if (sourcePathCount > 1500)
		throw LotusException("Source path in Common Header was too large: " + std::to_string(sourcePathCount));

	for (uint32_t x = 0; x < sourcePathCount; x++)
	{
		uint32_t curLen = *reinterpret_cast<const uint32_t*>(&file[filePos]);
		filePos += 4 + curLen;
	}

	uint32_t attributeLen = *reinterpret_cast<const uint32_t*>(&file[filePos]);
	filePos += 4 + attributeLen;

	if (attributeLen > 0)
		filePos += 1;
	
	filePos += 4;

	return filePos;
}

int
CommonHeaderReader::readHeader(const char* file, CommonHeader& header)
{
	size_t filePos = 0;

	std::memcpy(&header.hash[0], &file[0], 16);
	filePos += 16;

	uint32_t sourcePathCount = *reinterpret_cast<const uint32_t*>(&file[filePos]);
	filePos += 4;
	if (sourcePathCount > 1500)
		throw LotusException("Source path in Common Header was too large: " + std::to_string(sourcePathCount));

	header.paths.clear();
	for (uint32_t x = 0; x < sourcePathCount; x++)
	{
		uint32_t curLen = *reinterpret_cast<const uint32_t*>(&file[filePos]);
		filePos += 4;
		if (curLen > 200)
			throw LotusException("Source path length in Common Header was too large: " + std::to_string(curLen));
		
		header.paths.push_back(std::string(&file[filePos], curLen));
		filePos += curLen;
	}

	uint32_t attributeLen = *reinterpret_cast<const uint32_t*>(&file[filePos]);
	filePos += 4;

	header.attributes = std::string(&file[filePos], attributeLen);
	filePos += attributeLen;

	if (attributeLen > 0)
		filePos += 1;

	header.type = *reinterpret_cast<const uint32_t*>(&file[filePos]);
	filePos += 4;

	return filePos;
}

CommonHeader
CommonHeaderReader::readHeader(const char* file)
{
	CommonHeader header;
	readHeader(file, header);
	return header;
}

