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
        UNKNOWN             = 0,
        ANIM_RETARGET       = 1 << 0,
        CHARACTER_CODES     = 1 << 1,
        FONT                = 1 << 2,
        LIGHT_MAP           = 1 << 3,
        MISC                = 1 << 4,
        SHADER_PERMUTATION  = 1 << 5,
        SHADER              = 1 << 6,
        TEXTURE             = 1 << 7,
        VIDEO_TEXTURE       = 1 << 8,
    };

    inline PackageCategory operator|(PackageCategory a, PackageCategory b)
    {
        return static_cast<PackageCategory>(static_cast<int>(a) | static_cast<int>(b));
    }
};
