#include "LotusUtils.h"

LotusLib::PackageCategory
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
    if (name.rfind("Shader", 0) == 0)
        return PackageCategory::SHADER;
    if (name.rfind("Texture", 0) == 0)
        return PackageCategory::TEXTURE;
    if (name.rfind("VideoTexture", 0) == 0)
        return PackageCategory::VIDEO_TEXTURE;
    return UNKNOWN;
}
