#include "BinaryReaderBuffered.h"
#include "LotusLib/CommonHeader.h"
#include "LotusLib/Enums.h"
#include "LotusLib/Exceptions.h"
#include "LotusLib/PackageCollection.h"
#include "LotusLib/Package.h"
#include "LotusLib/PackageSplit.h"
#include "LotusLib/TOCTree.h"
#include "LotusLib/Utils.h"
#include "LotusLib/PackagesBin.h"

#include <cassert>
#include <cstdlib>
#include <tuple>
#include <iostream>

void testNodes(const PackageSplit& split, const FileNode& fileNode)
{
    const std::string root = getFullPath(fileNode);

    assert(split.fileExists(fileNode));
    assert(split.fileExists(getFullPath(fileNode)));
    assert(root.compare(getFullPath(split.getFileNode(fileNode))) == 0);
    assert(root.compare(getFullPath(split.getFileNode(getFullPath(fileNode)))) == 0);
    
    // Gotcha moment. Add to documentation
    if (fileNode.parentDir->parentNode != nullptr)
    {
        const std::string rootDir = getFullPath(*fileNode.parentDir);

        assert(split.dirExists(*fileNode.parentDir));
        assert(split.dirExists(getFullPath(*fileNode.parentDir)));
        assert(rootDir.compare(getFullPath(split.getDirNode(*fileNode.parentDir))) == 0);
        assert(rootDir.compare(getFullPath(split.getDirNode(getFullPath(*fileNode.parentDir)))) == 0);
    }
}

void testPackageIterator(const Package& pkg)
{
    for (const FileNode& fileNode : pkg)
    {
        assert(pkg.fileExists(PkgSplitType::HEADER, fileNode));
        assert(pkg.fileExists(PkgSplitType::HEADER, getFullPath(fileNode)));

        if (fileNode.parentDir->parentNode != nullptr)
        {
            assert(pkg.dirExists(PkgSplitType::HEADER, *fileNode.parentDir));
            assert(pkg.dirExists(PkgSplitType::HEADER, getFullPath(*fileNode.parentDir)));
        }
    }

    if (pkg.getName() != "Misc")
        return;

    // Cannot use the iterator like this
    //auto iter = pkg.getIter("/Lotus/Characters/Tenno/Excalibur");
    //while (iter++ != pkg.getIter())
    for (auto iter = pkg.getIter("/Lotus/Characters/Tenno/Excalibur"); iter != pkg.getIter(); iter++)
    {
        std::cout << getFullPath(*iter) << std::endl;
    }
}

void iterateAllFiles(std::filesystem::path cachePath, LotusLib::Game game)
{
    LotusLib::PackageCollection pkgs(cachePath, game);

    auto start = std::chrono::system_clock::now();
    size_t n = 0;
    for (LotusLib::Package& pkg : pkgs)
    {
        auto split = PkgSplitType::HEADER;
        if (!pkg.hasSplit(split))
            continue;
        PackageSplit pair = pkg.getSplit(split);
        for (const auto& file : pair)
        {
            //testNodes(pair, file);
            n++;
        }
        testPackageIterator(pkg);
    }
    auto end = std::chrono::system_clock::now();
    std::cout << n << " entries in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
}

void readmeFunction(std::filesystem::path cachePath)
{
    LotusLib::Game game = LotusLib::guessGame(cachePath);
    LotusLib::PackageCollection pkgCollection(cachePath, game);

    for (LotusLib::Package& pkg : pkgCollection)
    {
        // Iterate over virtual files inside the H Split
        for (const LotusLib::FileNode& fileNode : pkg)
        {
            // Quickly get the CommonHeader
            LotusLib::CommonHeader cHeader = pkg.readCommonHeader(fileNode);

            // An even quicker method to read the format
            uint32_t cHeaderFormat = pkg.readCommonHeaderFormat(fileNode);

            // Get the full file entry - CommonHeader, and H/B/F split
            LotusLib::FileEntry fullEntry = pkg.getFileEntry(fileNode);

            // Get the absolute path to the file
            std::string absPath = getFullPath(fileNode);
        }
    }

    LotusLib::Package miscPkg = pkgCollection.getPackage("Misc");
    std::vector<uint8_t> pkgsBinData = miscPkg.getFile(LotusLib::PkgSplitType::HEADER, "/Packages.bin");
    BinaryReader::BinaryReaderBuffered pkgsBinReader(std::move(pkgsBinData));

    LotusLib::PackagesBin pkgsBin;
    pkgsBin.initilize(pkgsBinReader);
    for (const auto& item : pkgsBin)
    {
        std::cout << "Entry path: " << item.first << std::endl;
        nlohmann::json params = pkgsBin.getParametersJson(item.first);
    }
}

int main()
{
    const std::tuple<std::string, Game> wf = {"/run/media/lotus/cb375011-523c-4cb3-aca8-67c8435a7541/Steam/steamapps/common/Warframe/Cache.Windows", Game::WARFRAME};
    const std::tuple<std::string, Game> sf = {"/run/media/lotus/cb375011-523c-4cb3-aca8-67c8435a7541/soulframe/Downloaded/Public/Cache.Windows/", Game::SOULFRAME};
    const std::tuple<std::string, Game> d2 = {"/run/media/lotus/cb375011-523c-4cb3-aca8-67c8435a7541/EvolutionEngine/DarknessII/Cache.Windows/", Game::DARKNESSII};
    const std::tuple<std::string, Game> st = {"/run/media/lotus/cb375011-523c-4cb3-aca8-67c8435a7541/EvolutionEngine/StarTrek/", Game::STARTREK};
    const std::tuple<std::string, Game> pe = {"/run/media/lotus/cb375011-523c-4cb3-aca8-67c8435a7541/EvolutionEngine/Warframe2013/", Game::WARFRAME_PE};
    const std::tuple<std::string, Game> game = wf;

    iterateAllFiles(std::get<0>(game), std::get<1>(game));

    return EXIT_SUCCESS;
}