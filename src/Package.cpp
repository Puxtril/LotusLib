#include "Package.h"

using namespace LotusLib;

Package::Package(std::filesystem::path pkgDir, std::string pkgName, Game game)
    : m_directory(pkgDir), m_name(pkgName), m_game(game), m_pkgs()
{
    loadPkgPairs();
    m_category = findPackageCategory();
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

void
Package::loadPkgPairs()
{
    for (int i = 0; i < 3; i++)
    {
        auto pair = getPairPath((PackageTrioType)i);
        if (std::filesystem::exists(std::get<0>(pair)) && std::filesystem::exists(std::get<1>(pair)))
        {
            m_pkgs[i].emplace(CachePair{std::get<0>(pair), std::get<1>(pair), m_game});
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

PackageCategory
Package::findPackageCategory()
{
    const std::string& name = getName();
    if (name.rfind("AnimRetarget", 0) == 0)
        return PackageCategory::ANIM_RETARGET;
    if (name.rfind("CharacterCodes", 0) == 0)
        return PackageCategory::CHARACTER_CODES;
    if (name.rfind("Font", 0) == 0)
        return PackageCategory::FONT;
    if (name.rfind("LightMap", 0) == 0)
        return PackageCategory::LIGHT_MAP;
    if (name.rfind("Misc", 0) == 0)
        return PackageCategory::MISC;
    if (name.rfind("Shader", 0) == 0)
        return PackageCategory::SHADER;
    if (name.rfind("Texture", 0) == 0)
        return PackageCategory::TEXTURE;
    if (name.rfind("VideoTexture", 0) == 0)
        return PackageCategory::VIDEO_TEXTURE;

    logWarn("Unable to match category for " + name);
    return UNKNOWN;
}