#pragma once

#include "Enums.h"

#include <string>

namespace LotusLib
{
    // Expects exact match - Upper Camel Case names
    // Intended to be used internally
    PackageCategory findPackageCategory(const std::string& name);

    std::string gameToString(Game game);
    std::string packageCategoryToString(PackageCategory pkgCategory);

    // Will be matched with lower-case names
    // Passed-in string must already be lower-case
    // Intended to match user input
    Game stringToGame(const std::string& gameStr);
    PackageCategory stringToPackageCategory(const std::string& pkgCategoryStr);
};