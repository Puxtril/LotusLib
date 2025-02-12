#pragma once

namespace LotusLib
{
    enum class Game : int
    {
        UNKNOWN = 0,
        SOULFRAME = 1,
        WARFRAME = 2,
        WARFRAME_PE = 4 // Pre-Ensmallening
    };

    inline Game operator|(Game a, Game b)
    {
        return static_cast<Game>(static_cast<int>(a) | static_cast<int>(b));
    }

    enum class PackageCategory : int
    {
        UNKNOWN = 0,
        ANIM_RETARGET = 1,
        CHARACTER_CODES = 2,
        FONT = 4,
        LIGHT_MAP = 8,
        MISC = 16,
        SHADER = 32,
        TEXTURE = 64,
        VIDEO_TEXTURE = 128
    };

    inline PackageCategory operator|(PackageCategory a, PackageCategory b)
    {
        return static_cast<PackageCategory>(static_cast<int>(a) | static_cast<int>(b));
    }
};
