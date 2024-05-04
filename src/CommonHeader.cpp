#include "CommonHeader.h"

int
LotusLib::commonHeaderFindLen(BinaryReader::BinaryReaderBuffered& reader)
{
	uint32_t length = 0;

	uint32_t sourcePathCount = reader.readUInt32();
	if (sourcePathCount > 1500)
		throw CommonHeaderError("Source path in Common Header was too large: " + std::to_string(sourcePathCount));
	length += 4;

	for (uint32_t x = 0; x < sourcePathCount; x++)
		length += 4 + reader.readUInt32();

	uint32_t attributeLen = reader.readUInt32();
	length += 4 + attributeLen;

	if (attributeLen > 0)
		length += 1;
	
	length += 4;

	return length;
}

uint32_t
LotusLib::commonHeaderReadFormat(BinaryReader::BinaryReaderBuffered& reader, bool seek)
{
	uint32_t length = 0;

	uint32_t sourcePathCount = reader.readUInt32();
	if (sourcePathCount > 1500)
		throw CommonHeaderError("Source path in Common Header was too large: " + std::to_string(sourcePathCount));
	length += 4;

	for (uint32_t x = 0; x < sourcePathCount; x++)
		length += 4 + reader.readUInt32();

	uint32_t attributeLen = reader.readUInt32();
	length += 4 + attributeLen;

	if (attributeLen > 0)
		length += 1;
	
	uint32_t format = reader.readUInt32();

	if (!seek)
		reader.seek(0, std::ios::beg);

	return format;
}

int
LotusLib::commonHeaderRead(BinaryReader::BinaryReaderBuffered& reader, CommonHeader& header)
{
	reader.readUInt8Array(&header.hash[0], 16);

	uint32_t sourcePathCount = reader.readUInt32();
	if (sourcePathCount > 1500)
		throw CommonHeaderError("Source path in Common Header was too large: " + std::to_string(sourcePathCount));

	for (uint32_t x = 0; x < sourcePathCount; x++)
	{
		uint32_t curLen = reader.readUInt32();
		if (curLen > 200)
			throw CommonHeaderError("Source path length in Common Header was too large: " + std::to_string(curLen));
		
		header.paths.push_back(reader.readAsciiString(curLen));
	}

	uint32_t attributeLen = reader.readUInt32();
	header.attributes = reader.readAsciiString(attributeLen);

	if (attributeLen > 0)
		reader.seek(1, std::ios::cur);

	header.type = reader.readUInt32();

	return reader.tell();
}

LotusLib::CommonHeader
LotusLib::commonHeaderRead(BinaryReader::BinaryReaderBuffered& reader)
{
	CommonHeader header;
	LotusLib::commonHeaderRead(reader, header);
	return header;
}
