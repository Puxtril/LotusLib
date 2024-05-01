#include "PackageCollection.h"

using namespace LotusLib;

PackageCollection::PackageCollection()
    : m_log(Logger::getInstance())
{}

PackageCollection::PackageCollection(std::filesystem::path pkgDir, bool isPostEnsmallening)
    : m_packageDir(pkgDir), m_isPostEnsmallening(isPostEnsmallening), m_log(Logger::getInstance())
{
    loadPackages();
}

void
PackageCollection::setData(std::filesystem::path pkgDir, bool isPostEnsmallening)
{
    m_packageDir = pkgDir;
    m_isPostEnsmallening = isPostEnsmallening;
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

const Package&
PackageCollection::operator[](const std::string& pkgName) const
{
    return m_pkgs[m_pkgMap.at(pkgName)];
}

Package&
PackageCollection::operator[](const std::string& pkgName)
{
    return m_pkgs[m_pkgMap.at(pkgName)];
}

const std::filesystem::path&
PackageCollection::getPath() const
{
    return m_packageDir;
}

bool
PackageCollection::isPostEnsmallening() const
{
    return m_isPostEnsmallening;
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
    m_log.info(spdlog::fmt_lib::format("Loaded {} packages: {}", m_pkgs.size(), pkgList));
}

void 
PackageCollection::loadPackage(std::string pkgName)
{
    m_pkgMap.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(pkgName),
        std::forward_as_tuple(static_cast<int>(m_pkgs.size()))
    );
    m_pkgs.emplace_back(m_packageDir, pkgName, m_isPostEnsmallening);
} 