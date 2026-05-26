#pragma once

#include "LotusLib/Enums.h"
#include "LotusLib/TOCEntry.h"
#include "LotusLib/TOCTree.h"

#include <algorithm>
#include <fstream>
#include <stack>
#include <string>
#include <sstream>

namespace LotusLib
{
    // Expects exact match - Upper Camel Case names
    // Intended to be used internally
    PackageCategory findPackageCategory(const std::string& name);

    // Not thread safe
    // https://en.cppreference.com/cpp/chrono/c/gmtime
    std::tm* parseDOSTimestamp(const int64_t& time);

    std::string getFullPath(const FileNode& fileNode);
    std::string getFullPath(const DirNode& dirNode);
    const DirNode* getChildDir(const DirNode& dirNode, const std::string& dirName);
    const FileNode* getChildFile(const DirNode& dirNode, const std::string& fileName);

    std::string gameToString(Game game);
    std::string packageCategoryToString(PackageCategory pkgCategory);
    std::string pkgSplitTypeToString(PkgSplitType split);
    char pkgSplitTypeToChar(PkgSplitType split);

    // Will be matched with lower-case names
    // Passed-in string must already be lower-case
    // Intended to match user input
    Game stringToGame(const std::string& gameStr);
    PackageCategory stringToPackageCategory(const std::string& pkgCategoryStr);

    // Max 10ms for Warframe/WarframePE
    Game guessGame(const std::string& pkgDir);

    // Good enough to identify various versions
    std::tuple<Game, std::string> gameIdentifier(const std::string& pkgDir);
};