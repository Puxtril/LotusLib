#pragma once

#include "CachePair.h"
#include "DirNode.h"
#include "FileNode.h"
#include "LotusPath.h"
#include "Package.h"
#include "PackageCollection.h"
#include "CommonHeader.h"
#include "BinaryReaderBuffered.h"
#include "PackagesBin.h"

#include <optional>

namespace LotusLib
{
    using FileRef = FileEntries::FileNode*;
    using DirRef = FileEntries::DirNode*;

    enum FileEntryReaderFlags : int
    {
        READ_COMMON_HEADER = 1,
        READ_H_CACHE = 2,
        READ_B_CACHE = 4,
        READ_F_CACHE = 8,
        READ_EXTRA_ATTRIBUTES = 16
    };

    struct FileExtraAttributes
    {
        LotusLib::LotusPath parent;
        std::string attributes;
    };

    // The easy-to-use file get-ter
    struct FileEntry
    {
        CommonHeader commonHeader;
        LotusPath internalPath;
        FileNode* metadata;
        std::string srcPkgName;
        BinaryReader::BinaryReaderBuffered headerData;
        BinaryReader::BinaryReaderBuffered bData;
        BinaryReader::BinaryReaderBuffered fData;
        FileExtraAttributes extra;
    };

    class PackageReader
    {
        Package* m_pkg;
        PackagesBin* m_packagesBin;
        Game game;

    public:
        PackageReader(Package* package, PackagesBin* packagesBin, Game game);

        CommonHeader getCommonHeader(LotusPath internalPath);
        // Assumes this is inside the H package
        // The iterator for this class is compatable with this method
        CommonHeader getCommonHeader(const FileNode* fileRef);

        // Optimized subset of CommonHeader, only reads the format number
        int getFileFormat(LotusPath internalPath);
        int getFileFormat(const FileNode* fileRef);

        FileEntry getFile(LotusPath internalPath);
        FileEntry getFile(LotusPath internalPath, int fileEntryReaderFlags);

        // If passing by FileNode*, it must be from the H cache
        FileEntry getFile(const FileNode* fileRef);
        FileEntry getFile(const FileNode* fileRef, int fileEntryReaderFlags);

        const FileNode* getFileNode(LotusPath internalPath, PackageTrioType trioType = PackageTrioType::H);
        const DirNode* getDirNode(LotusPath internalPath, PackageTrioType trioType = PackageTrioType::H);

        DirectoryTree::ConstFileIterator begin() const;
        DirectoryTree::ConstFileIterator end() const;
        DirectoryTree::FileIteratorTree getIter(LotusLib::LotusPath startingPath, PackageTrioType trioType = PackageTrioType::H) const;
        DirectoryTree::FileIteratorTree getIter(PackageTrioType trioType = PackageTrioType::H) const;

        const std::filesystem::path& getDirectory() const;
        const std::string& getName() const;
        Game getGame() const;
        PackageCategory getPkgCategory() const;

        void lsDir(const LotusPath& internalPath) const;
    
    private:
        // File doesn't exist in cache files, only Packages.bin
        FileEntry getFileOnlyPackagesBin(LotusLib::LotusPath internalpath);
    };

    class PackagesReader
    {
        std::vector<std::string> m_pkgNames;
        PackageCollection m_packgesDir;
        PackagesBin m_packagesBin;
        Game m_game;

    public:
        PackagesReader();
        PackagesReader(std::filesystem::path pkgDir, Game game = Game::WARFRAME);
        
        void setData(std::filesystem::path pkgDir, Game game = Game::WARFRAME);
        std::optional<PackageReader> getPackage(const std::string& name);
        void initilizePackagesBin();

        std::vector<std::string>::iterator begin();
        std::vector<std::string>::iterator end();

    private:
        std::vector<std::string> getPkgNames(PackageCollection& pkgDir);
    };
}