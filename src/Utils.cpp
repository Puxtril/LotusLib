#include "LotusLib/Utils.h"

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
LotusLib::getFullPath(const FileNode& fileNode)
{
    std::stack<const DirNode*> pathStack;
    std::stringstream pathStr;
    const DirNode* ptr = fileNode.parentDir;

    while (ptr->parentNode != nullptr)
    {
        pathStack.push(ptr);
        ptr = ptr->parentNode;
    }
    
    while (!pathStack.empty())
    {
        pathStr << '/' << pathStack.top()->name;
        pathStack.pop();
    }

    pathStr << '/' << fileNode.name;
    return pathStr.str();
}

std::string
LotusLib::getFullPath(const DirNode& dirNode)
{
    std::stack<const DirNode*> pathQueue;
    std::stringstream pathStr;
    const DirNode* ptr = dirNode.parentNode;

    while (ptr->parentNode != nullptr)
    {
        pathQueue.push(ptr);
        ptr = ptr->parentNode;
    }

    while (!pathQueue.empty())
    {
        pathStr << '/' << pathQueue.top()->name;
        pathQueue.pop();
    }

    pathStr << '/' << dirNode.name;
    return pathStr.str();
}

const DirNode*
LotusLib::getChildDir(const DirNode& dirNode, const std::string& dirName)
{
    for (DirNode* curNode : dirNode.childDirs)
	{
		if (dirNode.name.compare(curNode->name) == 0)
			return curNode;
	}
	return nullptr;
}

const FileNode*
LotusLib::getChildFile(const DirNode& dirNode, const std::string& fileName)
{
    for (FileNode* curNode : dirNode.childFiles)
	{
		if (dirNode.name.compare(curNode->name) == 0)
			return curNode;
	}
	return nullptr;
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
        case Game::DARKNESSII:
            return "Darkness II";
        case Game::STARTREK:
            return "Star Trek";
        case Game::DARKSECTOR:
            return "Dark Sector";
    }
    return "Unknown";
}

Game
LotusLib::stringToGame(const std::string& gameStr)
{
    std::string lower(gameStr);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "warframe")
        return Game::WARFRAME;
    if (lower == "soulframe")
        return Game::SOULFRAME;
    if (lower == "warframe_pe" || lower == "warframe-pe" || lower == "warframepe" || lower == "warframe pe")
        return Game::WARFRAME_PE;
    if (lower == "darkness2" || lower == "darkness 2" || lower == "darknessii" || lower == "darkness ii")
        return Game::DARKNESSII;
    if (lower == "startrek" || lower == "star trek")
        return Game::STARTREK;
    if (lower == "darksector" || lower == "dark sector")
        return Game::DARKSECTOR;
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

std::string
LotusLib::pkgSplitTypeToString(PkgSplitType split)
{
    switch(split)
    {
        case PkgSplitType::HEADER:
            return "Header";
        case PkgSplitType::BODY:
            return "Body";
        case PkgSplitType::FOOTER:
            return "Footer";
    }
    return "";
}

char
LotusLib::pkgSplitTypeToChar(PkgSplitType split)
{
    switch(split)
    {
        case PkgSplitType::HEADER:
            return 'H';
        case PkgSplitType::BODY:
            return 'B';
        case PkgSplitType::FOOTER:
            return 'F';
    }
    return '\0';
}

Game
LotusLib::guessGame(const std::string& pkgDir)
{
    std::filesystem::path pkgPath(pkgDir);
    
    // This exists for every game
    if (!std::filesystem::exists(pkgPath / "H.Misc.cache"))
        return Game::UNKNOWN;

    if (!std::filesystem::exists(pkgPath / "H.Misc.toc") && std::filesystem::exists(pkgPath / "H.BasePose.cache"))
        return Game::DARKSECTOR;

    // Dig into the Toc/Cache files further
    std::ifstream tocReader(pkgPath / "H.Misc.toc", std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
    int entryCount = static_cast<int>((tocReader.tellg() - (std::streampos)8) / (std::streampos)sizeof(RawTOCEntry));
	tocReader.seekg(8, std::ios_base::beg);

    // To guess between WARFRAME and WARFRAME_PE, we need to look at a file's bytes.
    // Both games contain ExcaliburBody_skel.fbx.
    int64_t excalOffset = 0;

    RawTOCEntry entryBuffer;
    LotusLib::Game game = Game::UNKNOWN;
    bool has_sf = false, has_lotus = false, has_d2 = false, has_st = false;

    // Search Toc entry names for root directories and excal's file offset
    for (int i = 0; i < entryCount; i++)
    {
        tocReader.read((char*)&entryBuffer, sizeof(RawTOCEntry));
        
        if (std::strncmp("SF", entryBuffer.name, sizeof(RawTOCEntry::name)) == 0)
            has_sf = true;
        else if (std::strncmp("Lotus", entryBuffer.name, sizeof(RawTOCEntry::name)) == 0)
            has_lotus = true;
        else if (std::strncmp("D2", entryBuffer.name, sizeof(RawTOCEntry::name)) == 0)
            has_d2 = true;
        else if (std::strncmp("ST", entryBuffer.name, sizeof(RawTOCEntry::name)) == 0)
            has_st = true;
        else if (std::strncmp("ExcaliburBody_skel.fbx", entryBuffer.name, sizeof(RawTOCEntry::name)) == 0)
            excalOffset = entryBuffer.cacheOffset;
    }

    if (has_sf)
    {
        game = Game::SOULFRAME;
    }
    else if (has_lotus)
    {
        std::ifstream cacheReader(pkgPath / "H.Misc.cache", std::ios_base::in | std::ios_base::binary);
        cacheReader.seekg(excalOffset, std::ios_base::beg);
        uint8_t readByte;
        cacheReader.read((char*)&readByte, 1);
        
        if (readByte == 0x80)
            game = Game::WARFRAME;
        else
            game = Game::WARFRAME_PE;
    }
    else if (has_d2)
    {
        game = Game::DARKNESSII;
    }
    else if (has_st)
    {
        game = Game::STARTREK;
    }

    return game;
}