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
    if (name.rfind("Emblem", 0) == 0)
        return PackageCategory::EMBLEM;
    if (name.rfind("BasePose", 0) == 0)
        return PackageCategory::BASEPOSE;
    if (name.rfind("Script", 0) == 0)
        return PackageCategory::SCRIPT;
    return PackageCategory::UNKNOWN;
}

std::tm*
LotusLib::parseDOSTimestamp(const int64_t& time)
{
    time_t epochTime = time / 10000000UL - 11644473600UL;
    std::time_t rawtime = static_cast<std::time_t>(epochTime);
    return std::gmtime(&rawtime);
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
    const DirNode* ptr = &dirNode;

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

    return pathStr.str();
}

const DirNode*
LotusLib::getChildDir(const DirNode& dirNode, const std::string& dirName)
{
    for (DirNode* curNode : dirNode.childDirs)
	{
		if (dirName.compare(curNode->name) == 0)
			return curNode;
	}
	return nullptr;
}

const FileNode*
LotusLib::getChildFile(const DirNode& dirNode, const std::string& fileName)
{
    for (FileNode* curNode : dirNode.childFiles)
	{
		if (fileName.compare(curNode->name) == 0)
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
        case Game::KEYSTONE:
            return "Keystone";
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
    if (lower == "keystone" || lower == "the amazing eternals" || lower == "amazing eternals")
        return Game::KEYSTONE;
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
        case PackageCategory::EMBLEM:
            return "Emblem";
        case PackageCategory::BASEPOSE:
            return "BasePose";
        case PackageCategory::SCRIPT:
            return "Script";
        case PackageCategory::SCENE:
            return "Scene";
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
    if (pkgCategoryStr == "emblem")
        return PackageCategory::EMBLEM;
    if (pkgCategoryStr == "basepose" || pkgCategoryStr == "base-pose" || pkgCategoryStr == "base_pose")
        return PackageCategory::BASEPOSE;
    if (pkgCategoryStr == "script")
        return PackageCategory::SCRIPT;
    if (pkgCategoryStr == "scene")
        return PackageCategory::SCENE;
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

std::string
LotusLib::fileTypeToString(const FileType fileType)
{
    switch(fileType)
    {
        case FileType::UNKNOWN:
            return "Unknown";
        case FileType::AUDIO:
            return "Audio";
        case FileType::ICON:
            return "Icon";
        case FileType::LANDSCAPE:
            return "Landscape";
        case FileType::LEVEL:
            return "Level";
        case FileType::LEVEL_STATIC:
            return "LevelStatic";
        case FileType::MATERIAL:
            return "Material";
        case FileType::MATERIAL_HLM:
            return "MaterialHLM";
        case FileType::MODEL_STATIC:
            return "ModelStatic";
        case FileType::MODEL_LEVEL1:
            return "ModelLevel1";
        case FileType::MODEL_LEVEL2:
            return "ModelLevel2";
        case FileType::MODEL_TERRAIN:
            return "ModelTerrain";
        case FileType::MODEL_RIGGED:
            return "ModelRigged";
        case FileType::MODEL_PACKED:
            return "ModelPacked";
        case FileType::MODEL_SM:
            return "ModelSM";
        case FileType::MODEL_DCM_HLOD:
            return "ModelDCMHLOD";
        case FileType::SHADER:
            return "Shader";
        case FileType::TEXTURE_1:
            return "Texture1";
        case FileType::TEXTURE_2:
            return "Texture2";
        case FileType::TEXTURE_3:
            return "Texture3";
        case FileType::TEXTURE_ROUGHNESS:
            return "TextureRoughness";
        case FileType::TEXTURE_LIGHTMAP_1:
            return "TextureLightmap1";
        case FileType::TEXTURE_SKYBOX:
            return "TextureSkybox";
        case FileType::TEXTURE_POSTPROCESS:
            return "TexturePostprocess";
        case FileType::TEXTURE_CUBEMAP_1:
            return "TextureCubemap1";
        case FileType::TEXTURE_CUBEMAP_2:
            return "TextureCubemap2";
        case FileType::TEXTURE_CUBEMAP_PARTIAL:
            return "TextureCubemapPartial";
        case FileType::TEXTURE_NORMALMAP:
            return "TextureNormalmap";
        case FileType::TEXTURE_PACKMAP:
            return "TexturePackmap";
        case FileType::TEXTURE_LANDSCAPE:
            return "TextureLandscape";
        case FileType::TEXTURE_DETAILSPACK:
            return "TextureDetailspack";
        case FileType::TEXTURE_FVL:
            return "TextureFVL";
        case FileType::TEXTURE_VRGB:
            return "TextureVRGB";
        case FileType::TEXTURE_ARRAY:
            return "TextureArray";
        case FileType::LIGHTMAP_1:
            return "Lightmap1";
        case FileType::LIGHTMAP_2:
            return "Lightmap2";
        case FileType::LIGHTMAP_3:
            return "Lightmap3";
        case FileType::LIGHTMAP_CUBEMAP_PARTIAL:
            return "LightmapCubemapPartial";
        case FileType::LIGHTMAP_FVL:
            return "LightmapFVL";
        case FileType::LIGHTMAP_VRGB:
            return "LightmapVRGB";
        case FileType::LEVEL_T:
            return "LevelT";
        case FileType::VERTEXCOLOR:
            return "VertexColor";
        case FileType::LIGHTMAP_CUBEMAP_SM:
            return "LightmapCubemapSM";
        case FileType::LIGHTMAP_HDR:
            return "LightmapHDR";
        case FileType::ANIMATION:
            return "Animation";
        case FileType::AUDIO_OLD:
            return "AudioOld";
        case FileType::BINK2:
            return "BinkVideo2";
        case FileType::SWF:
            return "AdobeFlash";
        }
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
    bool has_sf = false, has_lotus = false, has_d2 = false, has_st = false, has_ks = false;

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
        else if (std::strncmp("Keystone", entryBuffer.name, sizeof(RawTOCEntry::name)) == 0)
            has_ks = true;
        else if (std::strncmp("ExcaliburBody_skel.fbx", entryBuffer.name, sizeof(RawTOCEntry::name)) == 0)
            excalOffset = entryBuffer.cacheOffset;
    }

    LotusLib::Game game = Game::UNKNOWN;
    if (has_sf)
    {
        game = Game::SOULFRAME;
    }
    else if (has_ks)
    {
        game = Game::KEYSTONE;
    }
    else if (has_d2 && excalOffset == 0)
    {
        game = Game::DARKNESSII;
    }
    else if (has_st && excalOffset == 0)
    {
        game = Game::STARTREK;
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

    return game;
}

std::tuple<Game, std::string>
LotusLib::gameIdentifier(const std::string& pkgDir)
{
    std::filesystem::path pkgPath(pkgDir);
    
    // This exists for every game
    if (!std::filesystem::exists(pkgPath / "H.Misc.cache"))
        return {Game::UNKNOWN, gameToString(Game::UNKNOWN)};

    if (!std::filesystem::exists(pkgPath / "H.Misc.toc") && std::filesystem::exists(pkgPath / "H.BasePose.cache"))
        return {Game::DARKSECTOR, gameToString(Game::DARKSECTOR)};

    // Dig into the Toc/Cache files further
    std::ifstream tocReader(pkgPath / "H.Misc.toc", std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
    int entryCount = static_cast<int>((tocReader.tellg() - (std::streampos)8) / (std::streampos)sizeof(RawTOCEntry));
	tocReader.seekg(8, std::ios_base::beg);

    // To guess between WARFRAME and WARFRAME_PE, we need to look at a file's bytes.
    // Both games contain ExcaliburBody_skel.fbx.
    int64_t excalOffset = 0;

    // For games with various versions, use the timestamp of Packages.bin/Packages.cs to add context
    int64_t packagesTimestamp = 0;

    RawTOCEntry entryBuffer;
    bool has_sf = false, has_lotus = false, has_d2 = false, has_st = false, has_ks = false;

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
        else if (std::strncmp("Keystone", entryBuffer.name, sizeof(RawTOCEntry::name)) == 0)
            has_ks = true;
        else if (std::strncmp("ExcaliburBody_skel.fbx", entryBuffer.name, sizeof(RawTOCEntry::name)) == 0)
            excalOffset = entryBuffer.cacheOffset;

        if (std::strncmp("Packages.", entryBuffer.name, 9) == 0 && entryBuffer.timeStamp != 0)
            packagesTimestamp = entryBuffer.timeStamp;
    }

    std::tm* time = parseDOSTimestamp(packagesTimestamp);

    char buf[50];

    LotusLib::Game game = Game::UNKNOWN;
    if (has_ks)
    {
        return {Game::KEYSTONE, gameToString(Game::KEYSTONE)};
    }
    else if (has_d2 && excalOffset == 0)
    {
        return {Game::DARKNESSII, gameToString(Game::DARKNESSII)};
    }
    else if (has_st && excalOffset == 0)
    {
        return {Game::STARTREK, gameToString(Game::STARTREK)};
    }
    if (has_sf)
    {
        game = Game::SOULFRAME;
        std::strftime(buf, 50, "Soulframe %Y/%m", time);
    }
    else if (has_lotus)
    {
        std::ifstream cacheReader(pkgPath / "H.Misc.cache", std::ios_base::in | std::ios_base::binary);
        cacheReader.seekg(excalOffset, std::ios_base::beg);
        uint8_t readByte;
        cacheReader.read((char*)&readByte, 1);
        
        if (readByte == 0x80)
        {
            game = Game::WARFRAME;
            std::strftime(buf, 50, "Warframe %Y/%m", time);
        }
        else
        {
            game = Game::WARFRAME_PE;
            std::strftime(buf, 50, "WarframePE %Y/%m", time);
        }
    }
    return {game, std::string(buf)};
}