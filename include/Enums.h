#pragma once

namespace LotusLib
{
    enum Game : int
    {
        SOULFRAME = 1,
        WARFRAME = 2,
        WARFRAME_PE = 4 // Pre-Ensmallening
    };

    enum PackageCategory : int
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
};
