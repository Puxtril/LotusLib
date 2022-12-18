#pragma once

#include "CachePair.h"
#include "DirNode.h"
#include "FileNode.h"
#include "NodeDiffs.h"
#include "DirDupes.h"

#include <memory>

namespace LotusLib
{

	//! \brief Aims to provide statistical analysis
	//!
	//! .cache file is optional
	class CachePairMeta : public CachePair
	{
	public:
		CachePairMeta(std::filesystem::path tocPath, std::filesystem::path cachePath, bool isPostEnsmallening);
		CachePairMeta(std::filesystem::path tocPath, bool isPostEnsmallening);

		CachePairMeta(const CachePairMeta&) = delete;
		CachePairMeta& operator=(CachePairMeta&) = delete;

		Stats::DirDuplicateStats getDuplicateStats() const;
		// Returns a tree of diffs
		std::shared_ptr<Stats::DirDiff> findDiffs(const CachePair* other) const;

	private:
		void getDuplicateStats(Stats::DirDuplicateStats& stats, const FileEntries::DirNode* curDir) const;
		
		void _findDiffs(Stats::DirDiff* curDiff, const DirNode* oldDir, const DirNode* newDir) const;
		// A new/deleted dir was found
		// So everything below must also be new/deleted
		void addDirRecursive(Stats::DirDiff* newDir, Stats::NodeDiffType type) const;
	};
}
