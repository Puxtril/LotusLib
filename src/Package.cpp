#include "Package.h"

using namespace LotusLib;

Package::Package(std::filesystem::path pkgDir, std::string pkgName, bool isPostEnsmallening)
    : m_directory(pkgDir), m_name(pkgName), m_isPostEnsmallening(isPostEnsmallening), m_pkgs()
{
    loadPkgPairs();
}

std::array<std::optional<CachePair>, 3>::iterator
Package::begin()
{
    return m_pkgs.begin();
}

std::array<std::optional<CachePair>, 3>::iterator
Package::end()
{
    return m_pkgs.end();
}

std::array<std::optional<CachePair>, 3>::const_iterator
Package::begin() const
{
    return m_pkgs.begin();
}

std::array<std::optional<CachePair>, 3>::const_iterator
Package::end() const
{
    return m_pkgs.end();
}

const CachePair*
Package::getPair(PackageTrioType trioType) const
{
    if (!m_pkgs[(int)trioType].has_value())
        return nullptr;
    return &m_pkgs[(int)trioType].value();
}

CachePair*
Package::getPair(PackageTrioType trioType)
{
    if (!m_pkgs[(int)trioType].has_value())
        return nullptr;
    return &m_pkgs[(int)trioType].value();
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

bool
Package::isPostEnsmallening() const
{
    return m_isPostEnsmallening;
}

void
Package::loadPkgPairs()
{
    for (int i = 0; i < 3; i++)
    {
        auto pair = getPairPath((PackageTrioType)i);
        if (std::filesystem::exists(std::get<0>(pair)) && std::filesystem::exists(std::get<1>(pair)))
        {
            m_pkgs[i].emplace(CachePair{std::get<0>(pair), std::get<1>(pair), m_isPostEnsmallening});
        }
        else
        {
            logDebug(spdlog::fmt_lib::format("Package does not exist: {}", std::get<0>(pair).stem().string()));
        }
    }
}

std::tuple<std::filesystem::path, std::filesystem::path>
Package::getPairPath(PackageTrioType trioType)
{
    std::filesystem::path tocPath = m_directory;
    std::filesystem::path cachePath = m_directory;

    std::string trioChar = "";
    switch (trioType)
    {
    case PackageTrioType::H:
        trioChar = "H.";
        break;
    case PackageTrioType::F:
        trioChar = "F.";
        break;
    case PackageTrioType::B:
        trioChar = "B.";
        break;
    }

    tocPath /= trioChar + m_name + ".toc";
    cachePath /= trioChar + m_name + ".cache";

    return { tocPath, cachePath };
}