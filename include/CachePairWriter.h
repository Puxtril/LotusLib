#pragma once

#include "CachePair.h"

#include <filesystem>
#include <string>

namespace LotusLib
{
	//! \brief Manage modification of a .cache and .toc pair
	//!
	//! This is probably not releasing publically...
	class CachePairWriter : public CachePair
	{
	public:
		CachePairWriter(std::filesystem::path tocPath, std::filesystem::path cachePath, bool isPostEnsmallening);
	
		CachePairWriter(const CachePairWriter&) = delete;
		CachePairWriter operator=(CachePairWriter&) = delete;
	};
}
