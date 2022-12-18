#include "CachePairWriter.h"

using namespace LotusLib;

CachePairWriter::CachePairWriter(std::filesystem::path tocPath, std::filesystem::path cachePath, bool isPostEnsmallening)
	: CachePair(tocPath, cachePath, isPostEnsmallening)
{
}