#include "LotusLib/CommonHeader.h"

int
LotusLib::commonHeaderFindLen(BinaryReader::BinaryReader& reader, LotusLib::Game game)
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

	switch(game)
	{
		case LotusLib::Game::DARKNESSII:
		case LotusLib::Game::STARTREK:
			reader.seek(1, std::ios::cur);
			break;
		case LotusLib::Game::WARFRAME:
		case LotusLib::Game::WARFRAME_PE:
		case LotusLib::Game::SOULFRAME:
			reader.seek(4, std::ios::cur);
			break;
		default:
			throw LotusException("Cannot read CommonHeader from " + gameToString(game));
	}

	return (int)reader.tell();
}

uint32_t
LotusLib::commonHeaderReadFormat(BinaryReader::BinaryReader& reader, LotusLib::Game game, bool seek)
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
	switch(game)
	{
		case LotusLib::Game::DARKNESSII:
		case LotusLib::Game::STARTREK:
			format = reader.readUInt8();
			break;
		case LotusLib::Game::WARFRAME_PE:
		case LotusLib::Game::WARFRAME:
		case LotusLib::Game::SOULFRAME:
			format = reader.readUInt32() & 0x0000FFFF;
			break;
		default:
			throw LotusException("Cannot read CommonHeader from " + gameToString(game));
	}

	if (!seek)
		reader.seek(pos, std::ios::beg);

	return format;
}

int
LotusLib::commonHeaderRead(BinaryReader::BinaryReader& reader, CommonHeader& header, LotusLib::Game game)
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

	switch(game)
	{
		case LotusLib::Game::DARKNESSII:
		case LotusLib::Game::STARTREK:
			header.type = reader.readUInt8();
			break;
		case LotusLib::Game::WARFRAME_PE:
		case LotusLib::Game::WARFRAME:
		case LotusLib::Game::SOULFRAME:
			header.type = reader.readUInt32() & 0x0000FFFF;
			break;
		default:
			throw LotusException("Cannot read CommonHeader from " + gameToString(game));
	}

	return reader.tell();
}

LotusLib::CommonHeader
LotusLib::commonHeaderRead(BinaryReader::BinaryReader& reader, LotusLib::Game game)
{
	CommonHeader header;
	LotusLib::commonHeaderRead(reader, header, game);
	return header;
}
