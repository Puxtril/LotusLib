#pragma once

#include <cstdint>

namespace LotusLib {
	struct RawTOCEntry
	{
		int64_t cacheOffset;
		int64_t timeStamp;
		int32_t compressedLen;
		int32_t length;
		int32_t reserved;
		int32_t parentDirIndex;
		char name[64];
	};
}