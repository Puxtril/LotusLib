#pragma once

#include "CachePairReader.h"
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

    using BinaryReader = BinaryReader::BinaryReaderBuffered;

    struct FileEntry
    {
        CommonHeader commonHeader;
        LotusPath internalPath;
        BinaryReader headerData;
        BinaryReader bData;
        BinaryReader fData;
    };

    class PackageReader
    {
        Package<CachePairReader>& m_pkg;

    public:
        PackageReader(Package<CachePairReader>& package) : m_pkg(package) {}

        FileEntry getFile(LotusPath internalPath);
        FileEntry getFile(const FileNode& fileRef);

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
        PackagesReader(std::filesystem::path pkgDir) : m_packgesDir(pkgDir, true) { }
        
        PackageReader getPackage(std::string name);

        std::vector<Package<CachePairReader>>::iterator begin() { return m_packgesDir.begin(); }
		std::vector<Package<CachePairReader>>::iterator end() { return m_packgesDir.end(); }
		std::vector<Package<CachePairReader>>::const_iterator begin() const { return m_packgesDir.begin(); }
		std::vector<Package<CachePairReader>>::const_iterator end() const { return m_packgesDir.end(); }
    };
}