#include "LotusUtils.h"

using namespace LotusLib;

PackageCategory
LotusLib::findPackageCategory(const std::string& name)
{
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
    if (name.rfind("ShaderPermutation", 0) == 0)
        return PackageCategory::SHADER_PERMUTATION;
    if (name.rfind("Shader", 0) == 0)
        return PackageCategory::SHADER;
    if (name.rfind("Texture", 0) == 0)
        return PackageCategory::TEXTURE;
    if (name.rfind("VideoTexture", 0) == 0)
        return PackageCategory::VIDEO_TEXTURE;
    return PackageCategory::UNKNOWN;
}

std::string
LotusLib::gameToString(Game game)
{
    switch(game)
    {
        case Game::UNKNOWN:
            return "Unknown";
        case Game::SOULFRAME:
            return "Soulframe";
        case Game::WARFRAME:
            return "Warframe";
        case Game::WARFRAME_PE:
            return "Warframe (Pre-Ensmallening)";
    }
    return "Unknown";
}

Game
LotusLib::stringToGame(const std::string& gameStr)
{
    if (gameStr == "warframe")
        return Game::WARFRAME;
    if (gameStr == "soulframe")
        return Game::SOULFRAME;
    if (gameStr == "warframe_pe" || gameStr == "warframe-pe" || gameStr == "warframepe")
        return Game::WARFRAME_PE;
    return Game::UNKNOWN;
}

std::string
LotusLib::packageCategoryToString(PackageCategory pkgCategory)
{
    switch(pkgCategory)
    {
        case PackageCategory::UNKNOWN:
            return "Unknown";
        case PackageCategory::ANIM_RETARGET:
            return "AnimRetarget";
        case PackageCategory::CHARACTER_CODES:
            return "CharacterCodes";
        case PackageCategory::FONT:
            return "Font";
        case PackageCategory::LIGHT_MAP:
            return "LightMap";
        case PackageCategory::MISC:
            return "Misc";
        case PackageCategory::SHADER_PERMUTATION:
            return "ShaderPermutation";
        case PackageCategory::SHADER:
            return "Shader";
        case PackageCategory::TEXTURE:
            return "Texture";
        case PackageCategory::VIDEO_TEXTURE:
            return "VideoTexture";
    }
    return "Unknown";
}

PackageCategory
LotusLib::stringToPackageCategory(const std::string& pkgCategoryStr)
{
    if (pkgCategoryStr == "animretarget" || pkgCategoryStr == "anim-retarget" || pkgCategoryStr == "anim_retarget")
        return PackageCategory::ANIM_RETARGET;
    if (pkgCategoryStr == "charactercodes" || pkgCategoryStr == "character-codes" || pkgCategoryStr == "character_codes")
        return PackageCategory::CHARACTER_CODES;
    if (pkgCategoryStr == "font")
        return PackageCategory::FONT;
    if (pkgCategoryStr == "lightmap" || pkgCategoryStr == "light-map" || pkgCategoryStr == "light_map")
        return PackageCategory::LIGHT_MAP;
    if (pkgCategoryStr == "misc")
        return PackageCategory::MISC;
    if (pkgCategoryStr == "shaderpermutation")
        return PackageCategory::SHADER_PERMUTATION;
    if (pkgCategoryStr == "shader")
        return PackageCategory::SHADER;
    if (pkgCategoryStr == "texture")
        return PackageCategory::TEXTURE;
    if (pkgCategoryStr == "videotexture" || pkgCategoryStr == "video-texture" || pkgCategoryStr == "video_texture")
        return PackageCategory::VIDEO_TEXTURE;
    return PackageCategory::UNKNOWN;
}
