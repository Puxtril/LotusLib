#pragma once

#include "FileNode.h"
#include "DirNode.h"

#include <vector>

namespace LotusLib::Stats {
	enum NodeDiffType
	{
		Identical,
		New,
		Deleted,
		Modified
	};
	
	struct FileDiff
	{
		const FileEntries::FileNode* file;
		const NodeDiffType diffType;	
		FileDiff(const FileEntries::FileNode* file, NodeDiffType type);
	};
	
	class DirDiff
	{
		DirDiff* m_parent;
		const FileEntries::DirNode* m_dir;
		NodeDiffType m_diffType;
		bool m_hasSubdiffs;
		std::vector<FileDiff> m_fileDiffs;
		std::vector<DirDiff*> m_dirDiffs;

	public:	
		DirDiff();
		DirDiff(DirDiff* parent, const FileEntries::DirNode* dir, NodeDiffType type);

		DirDiff(const DirDiff&) = delete;
		DirDiff operator=(DirDiff&) = delete;
		~DirDiff();

		FileDiff* addFileDiff(const FileEntries::FileNode* file, NodeDiffType type);
		DirDiff* addDirDiff(const FileEntries::DirNode* dir, NodeDiffType type);

		std::vector<FileDiff> getFileDiffs();
		std::vector<DirDiff*> getDirDiffs();
		const std::vector<FileDiff> getFileDiffs() const;
		const std::vector<DirDiff*> getDirDiffs() const;

		const FileEntries::DirNode* getDir() const;
		const NodeDiffType& getDiffType() const;
		// Recursive
		bool hasSubdiffs() const;

	private:
		// If a subdirectory is found somehwere down the tree
		void foundSubdiffs();
	};
}
