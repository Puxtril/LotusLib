#include "LotusLib/PackageCollection.h"

using namespace LotusLib;

PackageCollection::PackageCollection(std::filesystem::path pkgDir, Game game)
    : m_packageDir(pkgDir), m_game(game)
{
    loadPackages();
}

std::vector<Package>::iterator
PackageCollection::begin()
{
    return m_pkgs.begin();
}

std::vector<Package>::iterator
PackageCollection::end()
{
    return m_pkgs.end();
}

std::vector<Package>::const_iterator
PackageCollection::begin() const
{
    return m_pkgs.begin();
}

std::vector<Package>::const_iterator
PackageCollection::end() const
{
    return m_pkgs.end();
}

Package
PackageCollection::getPackage(const std::string& pkgName) const
{
    if (m_pkgMap.count(pkgName) == 0)
        throw PackageNotFound(pkgName);
    return m_pkgs[m_pkgMap.at(pkgName)];
}

const std::filesystem::path&
PackageCollection::getPath() const
{
    return m_packageDir;
}

Game
PackageCollection::getGame() const
{
    return m_game;
}

const FileNode&
PackageCollection::getFileNode(const std::string& pkgName, PkgSplitType split, const std::string& internalPath) const
{
    if (m_pkgMap.count(pkgName) == 0)
        throw PackageNotFound(pkgName);
    return m_pkgs[m_pkgMap.at(pkgName)].getFileNode(split, internalPath);
}

const DirNode&
PackageCollection::getDirNode(const std::string& pkgName, PkgSplitType split, const std::string& internalPath) const
{
    if (m_pkgMap.count(pkgName) == 0)
        throw PackageNotFound(pkgName);
    return m_pkgs[m_pkgMap.at(pkgName)].getDirNode(split, internalPath);
}

size_t
PackageCollection::dirCount() const
{
    size_t total = 0;
    for (const Package& pkg : m_pkgs)
        total += pkg.dirCount();
    return total;
}

size_t
PackageCollection::dirCount(const std::string& pkgName) const
{
    if (m_pkgMap.count(pkgName) == 0)
        throw PackageNotFound(pkgName);
    return m_pkgs[m_pkgMap.at(pkgName)].dirCount();
}

size_t
PackageCollection::dirCount(const std::string& pkgName, PkgSplitType split) const
{
    if (m_pkgMap.count(pkgName) == 0)
        throw PackageNotFound(pkgName);
    return m_pkgs[m_pkgMap.at(pkgName)].dirCount(split);
}

size_t
PackageCollection::fileCount() const
{
    size_t total = 0;
    for (const Package& pkg : m_pkgs)
        total += pkg.fileCount();
    return total;
}

size_t
PackageCollection::fileCount(const std::string& pkgName) const
{
    if (m_pkgMap.count(pkgName) == 0)
        throw PackageNotFound(pkgName);
    return m_pkgs[m_pkgMap.at(pkgName)].fileCount();
}

size_t
PackageCollection::fileCount(const std::string& pkgName, PkgSplitType split) const
{
    if (m_pkgMap.count(pkgName) == 0)
        throw PackageNotFound(pkgName);
    return m_pkgs[m_pkgMap.at(pkgName)].fileCount(split);
}

size_t
PackageCollection::fileDupeCount() const
{
    size_t total = 0;
    for (const Package& pkg : m_pkgs)
        total += pkg.fileDupeCount();
    return total;
}

size_t
PackageCollection::fileDupeCount(const std::string& pkgName) const
{
    if (m_pkgMap.count(pkgName) == 0)
        throw PackageNotFound(pkgName);
    return m_pkgs[m_pkgMap.at(pkgName)].fileDupeCount();
}

size_t
PackageCollection::fileDupeCount(const std::string& pkgName, PkgSplitType split) const
{
    if (m_pkgMap.count(pkgName) == 0)
        throw PackageNotFound(pkgName);
    return m_pkgs[m_pkgMap.at(pkgName)].fileDupeCount(split);
}

std::vector<uint8_t>
PackageCollection::getFileUncompressed(const std::string& pkgName, PkgSplitType split, const std::string& internalPath) const
{
    if (m_pkgMap.count(pkgName) == 0)
        throw PackageNotFound(pkgName);
    return m_pkgs[m_pkgMap.at(pkgName)].getFileUncompressed(split, internalPath); 
}

std::vector<uint8_t>
PackageCollection::getFile(const std::string& pkgName, PkgSplitType split, const std::string& internalPath) const
{
    if (m_pkgMap.count(pkgName) == 0)
        throw PackageNotFound(pkgName);
    return m_pkgs[m_pkgMap.at(pkgName)].getFile(split, internalPath); 
}

void
PackageCollection::loadPackages()
{
    for (auto& f : std::filesystem::directory_iterator(m_packageDir))
    {
        std::string filename = f.path().filename().string();
        size_t start = filename.find('.');
        size_t end = filename.find('.', start + 1);

        if (start == std::string::npos || end == std::string::npos)
            continue;

        std::string pkgName = filename.substr(start + 1, end - 2);
        if (m_pkgMap.find(pkgName) == m_pkgMap.end())
            loadPackage(pkgName);
    }

    std::string pkgList = "";
    for (const auto& x : m_pkgs)
        pkgList += x.getName() + " ";
    logInfo(spdlog::fmt_lib::format("Loaded {} packages: {}", m_pkgs.size(), pkgList));
}

void 
PackageCollection::loadPackage(std::string pkgName)
{
    m_pkgMap.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(pkgName),
        std::forward_as_tuple(static_cast<int>(m_pkgs.size()))
    );
    m_pkgs.emplace_back(m_packageDir, pkgName, m_game);
} 