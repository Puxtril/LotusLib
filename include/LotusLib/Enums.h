#pragma once

namespace LotusLib
{
    enum class Game : int
    {
        UNKNOWN     = 0,
        SOULFRAME   = 1 << 0,
        WARFRAME    = 1 << 1,
        WARFRAME_PE = 1 << 2,
        STARTREK    = 1 << 3,
        DARKNESSII  = 1 << 4,
        DARKSECTOR  = 1 << 5,
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

    enum class PkgSplitType : int {
		HEADER = 0,
		BODY   = 1,
		FOOTER = 2,
	};
};
