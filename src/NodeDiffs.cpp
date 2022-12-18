#include "NodeDiffs.h"

using namespace LotusLib::Stats;

FileDiff::FileDiff(const FileEntries::FileNode* file, NodeDiffType type)
  : file(file), diffType(type)
{
}
	
DirDiff::DirDiff()
	: m_parent(nullptr), m_dir(nullptr), m_diffType(NodeDiffType::Identical)
{
}

DirDiff::DirDiff(DirDiff* parent, const FileEntries::DirNode* dir, NodeDiffType type)
  : m_parent(parent), m_dir(dir), m_diffType(type) 
{
}

DirDiff::~DirDiff()
{
	int count = m_dirDiffs.size();
	for (int x = 0; x < count; x++)
		delete m_dirDiffs[x];
}

FileDiff*
DirDiff::addFileDiff(const FileEntries::FileNode* file, NodeDiffType type)
{
	if (type != NodeDiffType::Identical)
		foundSubdiffs();
	m_fileDiffs.push_back({file, type});
	return &m_fileDiffs.back();
}

DirDiff*
DirDiff::addDirDiff(const FileEntries::DirNode* dir, NodeDiffType type)
{
	if (type != NodeDiffType::Identical)
		foundSubdiffs();
	m_dirDiffs.push_back(new DirDiff(this, dir, type));
	return m_dirDiffs.back();
}

std::vector<FileDiff>
DirDiff::getFileDiffs()
{
	return m_fileDiffs;
}

std::vector<DirDiff*>
DirDiff::getDirDiffs()
{
	return m_dirDiffs;
}

const std::vector<FileDiff>
DirDiff::getFileDiffs() const
{
	return m_fileDiffs;
}

const std::vector<DirDiff*>
DirDiff::getDirDiffs() const
{
	return m_dirDiffs;
}

const LotusLib::FileEntries::DirNode*
DirDiff::getDir() const
{
	return m_dir;
}

const NodeDiffType&
DirDiff::getDiffType() const
{
	return m_diffType;
}

bool
DirDiff::hasSubdiffs() const
{
	return m_hasSubdiffs;
}

void
DirDiff::foundSubdiffs()
{
	m_hasSubdiffs = true;
	if (m_parent != nullptr)
		m_parent->foundSubdiffs();
}
