#pragma once

#include "CachePairReader.h"
#include "DirNode.h"
#include "FileNode.h"
#include "LotusPath.h"
#include "Package.h"
#include "PackageCollection.h"
#include "CommonHeader.h"
#include "BinaryReaderBuffered.h"

namespace LotusLib
{
    using PackageSplitReader = std::shared_ptr<LotusLib::CachePairReader>;
    using FileRef = FileEntries::FileNode*;
    using DirRef = FileEntries::DirNode*;

    enum FileEntryReaderFlags : int
    {
        READ_COMMON_HEADER = 1,
        READ_H_CACHE = 2,
        READ_B_CACHE = 4,
        READ_F_CACHE = 8
    };

    class FileMeta
    {
        friend class PackageReader;
        const FileEntries::FileNode* fileNode;

    public:
        FileMeta(const FileEntries::FileNode* fileNode);
        FileMeta();
        
        bool isEmpty() const;
        const std::string& getName() const;
        int32_t getLen() const;
        int32_t getCompLen() const;
        int64_t getOffset() const;
        int64_t getTimeStamp() const;
        std::string getFullPath() const;
    };

    class DirMeta
    {
        friend class PackageReader;
        const FileEntries::DirNode* dirNode;

    public:
        DirMeta(const FileEntries::DirNode* dirNode);
        DirMeta();

        bool isEmpty() const;
        const std::string& getName() const;
        const DirMeta getParent() const;
        int getTocOffset() const;
        size_t getDirCount() const;
        size_t getFileCount() const;
        std::string getFullPath() const;

        const DirMeta getChildDir(int index) const;
        const DirMeta getChildDir(const std::string& name) const;
        const FileMeta getChildFile(int index) const;
        const FileMeta getChildFile(const std::string& name) const;
    };

    struct FileEntry
    {
        CommonHeader commonHeader;
        LotusPath internalPath;
        FileMeta metadata;
        BinaryReader::BinaryReaderBuffered headerData;
        BinaryReader::BinaryReaderBuffered bData;
        BinaryReader::BinaryReaderBuffered fData;
    };

    class PackageReader
    {
        Package<CachePairReader>& m_pkg;

    public:
        PackageReader(Package<CachePairReader>& package) : m_pkg(package) {}

        CommonHeader getCommonHeader(LotusPath internalPath);
        // Assumes this is inside the H package
        // The iterator for this class is compatable with this method
        CommonHeader getCommonHeader(const FileNode& fileRef);

        FileEntry getFile(LotusPath internalPath);
        FileEntry getFile(LotusPath internalPath, int fileEntryReaderFlags);
        FileEntry getFile(const FileNode* fileRef);
        FileEntry getFile(const FileNode* fileRef, int fileEntryReaderFlags);
        FileEntry getFile(const FileMeta& fileRef);
        FileEntry getFile(const FileMeta& fileRef, int fileEntryReaderFlags);

        FileMeta getFileMeta(LotusPath internalPath);
        DirMeta getDirMeta(LotusPath internalPath);

        DirectoryTree::ConstFileIterator begin() const;
		DirectoryTree::ConstFileIterator end() const;
        DirectoryTree::ConstFileIteratorTree getIter(LotusLib::LotusPath startingPath) const;
        DirectoryTree::ConstFileIteratorTree getIter() const;

        const std::filesystem::path& getDirectory() const;
		const std::string& getName() const;
		bool isPostEnsmallening() const;

        void lsDir(const LotusPath& internalPath) const;
    };

    class PackagesReader
    {
        PackageCollection<CachePairReader> m_packgesDir;

    public:
        PackagesReader() : m_packgesDir() {}
        PackagesReader(std::filesystem::path pkgDir) : m_packgesDir(pkgDir, true) { }
        
        void setData(std::filesystem::path pkgDir) { m_packgesDir.setData(pkgDir, true); }
        PackageReader getPackage(std::string name);

        std::vector<Package<CachePairReader>>::iterator begin() { return m_packgesDir.begin(); }
		std::vector<Package<CachePairReader>>::iterator end() { return m_packgesDir.end(); }
		std::vector<Package<CachePairReader>>::const_iterator begin() const { return m_packgesDir.begin(); }
		std::vector<Package<CachePairReader>>::const_iterator end() const { return m_packgesDir.end(); }
    };
}