#include "CommonHeader.h"

int
LotusLib::commonHeaderFindLen(BinaryReader::BinaryReaderBuffered& reader, LotusLib::Game game)
{
	if (game == LotusLib::Game::WARFRAME || game == LotusLib::Game::WARFRAME_PE || game == LotusLib::Game::SOULFRAME)
		reader.seek(16, std::ios::beg);

	uint32_t sourcePathCount = reader.readUInt32();
	if (sourcePathCount > 1500)
		throw CommonHeaderError("Source path in Common Header was too large: " + std::to_string(sourcePathCount));

	for (uint32_t x = 0; x < sourcePathCount; x++)
	{
		uint32_t curLen = reader.readUInt32();
		if (curLen > 200)
			throw CommonHeaderError("Source path length in Common Header was too large: " + std::to_string(curLen));
		
		reader.seek(curLen, std::ios::cur);
	}

	uint32_t attributeLen = reader.readUInt32();
	reader.seek(attributeLen, std::ios::cur);

	if (attributeLen > 0)
		reader.seek(1, std::ios::cur);

	if (game == LotusLib::Game::WARFRAME || game == LotusLib::Game::WARFRAME_PE || game == LotusLib::Game::SOULFRAME)
		reader.seek(4, std::ios::cur);
	else
		reader.seek(1, std::ios::cur);

	return (int)reader.tell();
}

uint32_t
LotusLib::commonHeaderReadFormat(BinaryReader::BinaryReaderBuffered& reader, LotusLib::Game game, bool seek)
{
	size_t pos = reader.tell();

	if (game == LotusLib::Game::WARFRAME || game == LotusLib::Game::WARFRAME_PE || game == LotusLib::Game::SOULFRAME)
		reader.seek(16, std::ios::beg);

	uint32_t sourcePathCount = reader.readUInt32();
	if (sourcePathCount > 1500)
		throw CommonHeaderError("Source path in Common Header was too large: " + std::to_string(sourcePathCount));

	for (uint32_t x = 0; x < sourcePathCount; x++)
	{
		uint32_t curLen = reader.readUInt32();
		if (curLen > 200)
			throw CommonHeaderError("Source path length in Common Header was too large: " + std::to_string(curLen));
		
		reader.seek(curLen, std::ios::cur);
	}

	uint32_t attributeLen = reader.readUInt32();
	reader.seek(attributeLen, std::ios::cur);

	if (attributeLen > 0)
		reader.seek(1, std::ios::cur);

	uint32_t format;
	if (game == LotusLib::Game::WARFRAME || game == LotusLib::Game::WARFRAME_PE || game == LotusLib::Game::SOULFRAME)
		format = reader.readUInt32() & 0x0000FFFF;
	else
	 	format = reader.readUInt8();

	if (!seek)
		reader.seek(pos, std::ios::beg);

	return format;
}

int
LotusLib::commonHeaderRead(BinaryReader::BinaryReaderBuffered& reader, CommonHeader& header, LotusLib::Game game)
{
	if (game == LotusLib::Game::WARFRAME || game == LotusLib::Game::WARFRAME_PE || game == LotusLib::Game::SOULFRAME)
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

	if (game == LotusLib::Game::WARFRAME || game == LotusLib::Game::WARFRAME_PE || game == LotusLib::Game::SOULFRAME)
		header.type = reader.readUInt32() & 0x0000FFFF;
	else
	 	header.type = reader.readUInt8();

	return reader.tell();
}

LotusLib::CommonHeader
LotusLib::commonHeaderRead(BinaryReader::BinaryReaderBuffered& reader, LotusLib::Game game)
{
	CommonHeader header;
	LotusLib::commonHeaderRead(reader, header, game);
	return header;
}
