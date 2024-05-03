#include "CommonHeader.h"

int
LotusLib::CHFindLen(BinaryReader::BinaryReaderBuffered& reader)
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

int
LotusLib::CHRead(BinaryReader::BinaryReaderBuffered& reader, CommonHeader& header)
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
LotusLib::CHRead(BinaryReader::BinaryReaderBuffered& reader)
{
	CommonHeader header;
	LotusLib::CHRead(reader, header);
	return header;
}
