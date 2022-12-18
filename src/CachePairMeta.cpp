#include "CachePairMeta.h"

using namespace LotusLib;

CachePairMeta::CachePairMeta(std::filesystem::path tocPath, std::filesystem::path cachePath, bool isPostEnsmallening)
	: CachePair(tocPath, cachePath, isPostEnsmallening)
{
}

CachePairMeta::CachePairMeta(std::filesystem::path tocPath, bool isPostEnsmallening)
	: CachePair(tocPath, std::filesystem::path(), isPostEnsmallening)
{
}

Stats::DirDuplicateStats
CachePairMeta::getDuplicateStats() const
{
	Stats::DirDuplicateStats stats;
	getDuplicateStats(stats, m_dirTree.getDirEntry("/"));
	return stats;
}

void
CachePairMeta::getDuplicateStats(Stats::DirDuplicateStats& stats, const FileEntries::DirNode* curDir) const
{
	for (size_t i = 0; i < curDir->getFileCount(); i++)
	{
		const FileEntries::FileNode* x = curDir->getChildFile(i);
		if (x->getOffset() < stats.firstEntryOffset)
			stats.firstEntryOffset = x->getOffset();
		stats.dupliateCount++;
		stats.compTotalSize += x->getCompLen();
		stats.totalSize += x->getLen();
	}

	for (size_t i = 0; i < curDir->getDirCount(); i++)
	{
		getDuplicateStats(stats, curDir->getChildDir(i));
	}
}

std::shared_ptr<Stats::DirDiff>
CachePairMeta::findDiffs(const CachePair* other) const
{
	auto rootDiff = std::make_shared<Stats::DirDiff>();
	_findDiffs(rootDiff.get(), m_dirTree.getDirEntry("/"), other->getDirEntry("/"));
	return rootDiff;
}

void
CachePairMeta::_findDiffs(Stats::DirDiff* curDiff, const FileEntries::DirNode* oldDir, const FileEntries::DirNode* newDir) const
{
	std::vector<std::tuple<Stats::DirDiff*, const FileEntries::DirNode*, const FileEntries::DirNode*>> matchingDirs;
	
	// Algorithm for directories and files:
	// First loop over all old directory elements, comparing to new dir elements.
	// If element count differs, also loop over new directory elements.
	// Small optimization that will be applied to most directories,
	//	which will be identica.
	int foundMatchDirCount = 0;
	for (int x = 0; x < (int)oldDir->getDirCount(); x++)
	{
		const FileEntries::DirNode* curOldDir = oldDir->getChildDir(x);
		const FileEntries::DirNode* curNewDir = newDir->getChildDir(curOldDir->getName());
		
		// Deleted
		if (curNewDir == nullptr)
		{
			Stats::DirDiff* newDiff = curDiff->addDirDiff(curOldDir, Stats::NodeDiffType::Deleted);
			addDirRecursive(newDiff, Stats::NodeDiffType::Deleted);
		}
		// Identical
		else
		{
			Stats::DirDiff* newDiff = curDiff->addDirDiff(curOldDir, Stats::NodeDiffType::Identical);
			matchingDirs.push_back(std::make_tuple(newDiff, curOldDir, curNewDir));
			foundMatchDirCount++;
		}
	}
	if (foundMatchDirCount != (int)newDir->getDirCount())
	{
		for (int x = 0; x < (int)newDir->getDirCount(); x++)
		{
			const FileEntries::DirNode* curNewDir = newDir->getChildDir(x);
			const FileEntries::DirNode* curOldDir = oldDir->getChildDir(curNewDir->getName());

			// New
			if (curOldDir == nullptr)
			{
				Stats::DirDiff* newDiff = curDiff->addDirDiff(curOldDir, Stats::NodeDiffType::New);
				addDirRecursive(newDiff, Stats::NodeDiffType::New);
			}
		}
	}

	// Files
	int foundMatchFileCount = 0;
	for (int x = 0; x < (int)oldDir->getFileCount(); x++)
	{
		const FileEntries::FileNode* curOldFile = oldDir->getChildFile(x);
		const FileEntries::FileNode* curNewFile = newDir->getChildFile(curOldFile->getName());
		
		// Deleted
		if (curNewFile == nullptr)
			curDiff->addFileDiff(curOldFile, Stats::NodeDiffType::Deleted);
		// Identical
		else
		{
			if (curNewFile->getTimeStamp() != curOldFile->getTimeStamp())
			{
				// Modified
				curDiff->addFileDiff(curNewFile, Stats::NodeDiffType::Modified);
			}
			foundMatchFileCount++;
		}
	}
	if (foundMatchFileCount != (int)newDir->getFileCount())
	{
		for (int x = 0; x < (int)newDir->getFileCount(); x++)
		{
			const FileEntries::FileNode* curNewFile = newDir->getChildFile(x);
			const FileEntries::FileNode* curOldFile = oldDir->getChildFile(curNewFile->getName());
			
			// New
			if (curOldFile == nullptr)
				curDiff->addFileDiff(curNewFile, Stats::NodeDiffType::New);
		}
	}

	// Recurse
	for (auto& x : matchingDirs)
	{
		_findDiffs(std::get<0>(x), std::get<1>(x), std::get<2>(x));
	}
}

void
CachePairMeta::addDirRecursive(Stats::DirDiff* newDir, Stats::NodeDiffType type) const
{
	for (int x = 0; x < (int)newDir->getDir()->getFileCount(); x++)
	{
		const FileEntries::FileNode* node = newDir->getDir()->getChildFile(x);
		newDir->addFileDiff(node, type);
	}
	
	for (int x = 0; x < (int)newDir->getDir()->getDirCount(); x++)
	{
		const FileEntries::DirNode* node = newDir->getDir()->getChildDir(x);
		Stats::DirDiff* newDiff = newDir->addDirDiff(node, type);
		addDirRecursive(newDiff, type);
	}
}
