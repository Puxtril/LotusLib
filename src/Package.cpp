#include "LotusLib/Package.h"

using namespace LotusLib;

Package::Package(std::filesystem::path pkgDir, std::string pkgName, Game game)
    : m_directory(pkgDir), m_name(pkgName), m_game(game), m_pkgs()
{
    loadPkgSplits();
    m_category = findPackageCategory(getName());
    if (m_category == PackageCategory::UNKNOWN)
        logWarn("Unable to match category for " + getName());
}

bool
Package::hasSplit(PkgSplitType pkgSplit) const
{
    if (!m_pkgs[(int)pkgSplit])
        return false;
    return true;
}

PackageSplit
Package::getSplit(PkgSplitType pkgSplit) const
{
    std::optional<PackageSplit> split = m_pkgs[(int)pkgSplit];
    if (!split)
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(pkgSplit));
    return split.value();
}

std::vector<FileNode>::const_iterator
Package::begin() const
{
	return m_pkgs[(int)PkgSplitType::HEADER]->begin();
}

std::vector<FileNode>::const_iterator
Package::end() const
{
	return m_pkgs[(int)PkgSplitType::HEADER]->end();
}

TOCTreeIterator
Package::getIter(const std::string& path) const
{
	return m_pkgs[(int)PkgSplitType::HEADER]->getIter(path);
}

TOCTreeIterator
Package::getIter() const
{
	return m_pkgs[(int)PkgSplitType::HEADER]->getIter();
}

const std::filesystem::path&
Package::getDirectory() const
{
    return m_directory;
}

const std::string&
Package::getName() const
{
    return m_name;
}

Game
Package::getGame() const
{
    return m_game;
}

PackageCategory
Package::getPkgCategory() const
{
    return m_category;
}

FileEntry
Package::getFileEntry(const std::string& internalPath)
{
    return getFileEntry(m_pkgs[(int)PkgSplitType::HEADER]->getFileNode(internalPath));
}

FileEntry
Package::getFileEntry(const FileNode& entry)
{
    FileEntry ret;

    std::vector<uint8_t> headerData = getFile(PkgSplitType::HEADER, entry);
    ret.header = BinaryReader::Buffered(std::move(headerData));
    ret.commonHeader = commonHeaderRead(ret.header, m_game);

    try
    {
        std::vector<uint8_t> bodyData = getFile(PkgSplitType::BODY, getFileNode(PkgSplitType::BODY, entry));
        ret.body = BinaryReader::Buffered(std::move(bodyData));
    }
    catch (InternalEntryNotFound&) { }

    try
    {
        std::vector<uint8_t> footerData = getFile(PkgSplitType::FOOTER, getFileNode(PkgSplitType::FOOTER, entry));
        ret.footer = BinaryReader::Buffered(std::move(footerData));
    }
    catch (InternalEntryNotFound&) { }

    return ret;
}

bool
Package::fileExists(PkgSplitType split, const std::string& internalPath) const
{
    if (!m_pkgs[(int)split])
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(split));
    return m_pkgs[(int)split]->fileExists(internalPath);
}

bool
Package::dirExists(PkgSplitType split, const std::string& internalPath) const
{
    if (!m_pkgs[(int)split])
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(split));
    return m_pkgs[(int)split]->dirExists(internalPath);
}

bool
Package::fileExists(PkgSplitType split, const FileNode& fileNode) const
{
    if (!m_pkgs[(int)split])
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(split));
    return m_pkgs[(int)split]->fileExists(fileNode);
}

bool
Package::dirExists(PkgSplitType split, const DirNode& dirNode) const
{
    if (!m_pkgs[(int)split])
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(split));
    return m_pkgs[(int)split]->dirExists(dirNode);
}


const FileNode&
Package::getFileNode(PkgSplitType split, const FileNode& fileNode) const
{
    if (!m_pkgs[(int)split])
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(split));
    return m_pkgs[(int)split]->getFileNode(fileNode);
}

const DirNode&
Package::getDirNode(PkgSplitType split, const DirNode& dirNode) const
{
    if (!m_pkgs[(int)split])
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(split));
    return m_pkgs[(int)split]->getDirNode(dirNode);
}

const FileNode&
Package::getFileNode(PkgSplitType split, const std::string& internalPath) const
{
    if (!m_pkgs[(int)split])
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(split));
    return m_pkgs[(int)split]->getFileNode(internalPath);
}

const DirNode&
Package::getDirNode(PkgSplitType split, const std::string& internalPath) const
{
    if (!m_pkgs[(int)split])
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(split));
    return m_pkgs[(int)split]->getDirNode(internalPath); 
}

size_t
Package::dirCount(PkgSplitType split) const
{
    if (!m_pkgs[(int)split])
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(split));
    return m_pkgs[(int)split]->dirCount(); 
}

size_t
Package::dirCount() const
{
    size_t total = 0;
    if (m_pkgs[0])
        total += m_pkgs[0]->dirCount();
    if (m_pkgs[1])
        total += m_pkgs[0]->dirCount();
    if (m_pkgs[2])
        total += m_pkgs[0]->dirCount();
    return total;
}

size_t
Package::fileCount(PkgSplitType split) const
{
    if (!m_pkgs[(int)split])
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(split));
    return m_pkgs[(int)split]->fileCount();
}

size_t
Package::fileCount() const
{
    size_t total = 0;
    if (m_pkgs[0])
        total += m_pkgs[0]->fileCount();
    if (m_pkgs[1])
        total += m_pkgs[0]->fileCount();
    if (m_pkgs[2])
        total += m_pkgs[0]->fileCount();
    return total;
}

size_t
Package::fileDupeCount(PkgSplitType split) const
{
    if (!m_pkgs[(int)split])
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(split));
    return m_pkgs[(int)split]->fileDupeCount();
}

size_t
Package::fileDupeCount() const
{
    size_t total = 0;
    if (m_pkgs[0])
        total += m_pkgs[0]->fileDupeCount();
    if (m_pkgs[1])
        total += m_pkgs[0]->fileDupeCount();
    if (m_pkgs[2])
        total += m_pkgs[0]->fileDupeCount();
    return total;
}

std::vector<uint8_t>
Package::getFile(PkgSplitType split, const std::string& internalPath) const
{
    if (!m_pkgs[(int)split])
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(split));
    return m_pkgs[(int)split]->getFile(internalPath);
}

std::vector<uint8_t>
Package::getFile(PkgSplitType split, const FileNode& entry) const
{
    if (!m_pkgs[(int)split])
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(split));
    return m_pkgs[(int)split]->getFile(entry);
}

std::vector<uint8_t>
Package::getFileUncompressed(PkgSplitType split, const std::string& internalPath) const
{
    if (!m_pkgs[(int)split])
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(split));
    return m_pkgs[(int)split]->getFileUncompressed(internalPath);
}

std::vector<uint8_t>
Package::getFileUncompressed(PkgSplitType split, const FileNode& entry) const
{
    if (!m_pkgs[(int)split])
        throw PackageSplitNotFound(m_name, pkgSplitTypeToChar(split));
    return m_pkgs[(int)split]->getFileUncompressed(entry);
}

CommonHeader
Package::readCommonHeader(const FileNode& entry)
{
	return m_pkgs[(int)PkgSplitType::HEADER]->readCommonHeader(entry);
}

uint32_t
Package::readCommonHeaderFormat(const FileNode& entry)
{
	return m_pkgs[(int)PkgSplitType::HEADER]->readCommonHeaderFormat(entry);
}

void
Package::loadPkgSplits()
{
    for (int i = 0; i < 3; i++)
    {
        auto pair = getSplitPath((PkgSplitType)i);
        // Only check TOC file existance
        if (std::filesystem::exists(std::get<0>(pair)))
        {
            m_pkgs[i].emplace(PackageSplit{std::get<0>(pair), std::get<1>(pair), m_game, (PkgSplitType)i});
        }
    }
}

std::tuple<std::filesystem::path, std::filesystem::path>
Package::getSplitPath(PkgSplitType pkgSplit)
{
    std::filesystem::path tocPath = m_directory;
    std::filesystem::path cachePath = m_directory;

    std::string splitChar = "";
    switch (pkgSplit)
    {
    case PkgSplitType::HEADER:
        splitChar = "H.";
        break;
    case PkgSplitType::BODY:
        splitChar = "B.";
        break;
    case PkgSplitType::FOOTER:
        splitChar = "F.";
        break;
    }

    tocPath /= splitChar + m_name + ".toc";
    cachePath /= splitChar + m_name + ".cache";

    return { tocPath, cachePath };
}
