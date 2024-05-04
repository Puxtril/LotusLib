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
        BinaryReader::BinaryReaderBuffered headerData;
        BinaryReader::BinaryReaderBuffered bData;
        BinaryReader::BinaryReaderBuffered fData;
        FileExtraAttributes extra;
    };

    class PackageReader
    {
        Package* m_pkg;
        PackagesBin* m_packagesBin;

    public:
        PackageReader(Package* package, PackagesBin* packagesBin) : m_pkg(package), m_packagesBin(packagesBin) {}

        CommonHeader getCommonHeader(LotusPath internalPath);
        // Assumes this is inside the H package
        // The iterator for this class is compatable with this method
        CommonHeader getCommonHeader(const FileNode& fileRef);

        FileEntry getFile(LotusPath internalPath);
        FileEntry getFile(LotusPath internalPath, int fileEntryReaderFlags);
        FileEntry getFile(const FileNode* fileRef);
        FileEntry getFile(const FileNode* fileRef, int fileEntryReaderFlags);

        const FileNode* getFileNode(LotusPath internalPath);
        const DirNode* getDirNode(LotusPath internalPath);

        DirectoryTree::ConstFileIterator begin() const;
		DirectoryTree::ConstFileIterator end() const;
        DirectoryTree::FileIteratorTree getIter(LotusLib::LotusPath startingPath) const;
        DirectoryTree::FileIteratorTree getIter() const;

        const std::filesystem::path& getDirectory() const;
		const std::string& getName() const;
		bool isPostEnsmallening() const;

        void lsDir(const LotusPath& internalPath) const;
    
    private:
        // File doesn't exist in cache files, only Packages.bin
        FileEntry getFileOnlyPackagesBin(LotusLib::LotusPath internalpath);
    };

    class PackagesReader
    {
        PackageCollection m_packgesDir;
        PackagesBin m_packagesBin;

    public:
        PackagesReader() : m_packgesDir() {}
        PackagesReader(std::filesystem::path pkgDir) : m_packgesDir(pkgDir, true) { }
        
        void setData(std::filesystem::path pkgDir) { m_packgesDir.setData(pkgDir, true); }
        PackageReader getPackage(std::string name);
        void initilizePackagesBin();

        std::vector<Package>::iterator begin() { return m_packgesDir.begin(); }
		std::vector<Package>::iterator end() { return m_packgesDir.end(); }
		std::vector<Package>::const_iterator begin() const { return m_packgesDir.begin(); }
		std::vector<Package>::const_iterator end() const { return m_packgesDir.end(); }
    };
}