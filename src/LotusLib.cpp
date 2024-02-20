#include "LotusLib.h"

using namespace LotusLib;

FileEntry
PackageReader::getFile(LotusPath internalPath)
{
    PackageSplitReader split = m_pkg[PackageTrioType::H];
    split->readToc();
    FileRef fileRef = split->getFileEntry(internalPath);
    return getFile(*fileRef);
}

FileEntry
PackageReader::getFile(const FileNode& fileRef)
{
    FileEntry entry;
    entry.internalPath = fileRef.getFullPath();
    PackageSplitReader splitH = m_pkg[PackageTrioType::H];
    splitH->readToc();
    entry.metadata = FileMeta(splitH->getFileEntry(fileRef.getFullPath()));

    std::vector<uint8_t> dataHeader = splitH->getDataAndDecompress(&fileRef);
    entry.headerData = BinaryReader(std::move(dataHeader));
    entry.commonHeader = CHRead(entry.headerData);

    PackageSplitReader splitB = m_pkg[PackageTrioType::B];
    if (splitB)
    {
        splitB->readToc();
        try
        {
            FileRef entryB = splitB->getFileEntry(entry.internalPath);
            std::vector<uint8_t> dataB = splitB->getDataAndDecompress(entryB);
            entry.bData = BinaryReader(std::move(dataB));
        }
        catch (std::runtime_error&) { }
    }

    PackageSplitReader splitF = m_pkg[PackageTrioType::F];
    if (splitF)
    {
        splitF->readToc();
        try
        {
            FileRef entryF = splitF->getFileEntry(entry.internalPath);
            std::vector<uint8_t> dataF = splitF->getDataAndDecompress(entryF);
            entry.fData = BinaryReader(std::move(dataF));
        }
        catch (std::runtime_error&) { }
    }

    return entry;
}

FileMeta
PackageReader::getFileMeta(LotusPath internalPath)
{
    PackageSplitReader split = m_pkg[PackageTrioType::H];
    split->readToc();
    LotusLib::FileEntries::FileNode* fileRef = split->getFileEntry(internalPath);
    return FileMeta(fileRef);
}

DirectoryTree::ConstFileIterator
PackageReader::begin() const
{
    PackageSplitReader splitH = m_pkg[PackageTrioType::H];
    splitH->readToc();
    return splitH->begin();
}

DirectoryTree::ConstFileIterator
PackageReader::end() const
{
    PackageSplitReader splitH = m_pkg[PackageTrioType::H];
    splitH->readToc();
    return splitH->end();
}

DirectoryTree::ConstFileIteratorTree
PackageReader::getIter(LotusLib::LotusPath startingPath) const
{
    PackageSplitReader splitH = m_pkg[PackageTrioType::H];
    splitH->readToc();
    return splitH->getIter(startingPath);
}

DirectoryTree::ConstFileIteratorTree
PackageReader::getIter() const
{
    PackageSplitReader splitH = m_pkg[PackageTrioType::H];
    splitH->readToc();
    return splitH->getIter();
}

const std::filesystem::path&
PackageReader::getDirectory() const
{
    return m_pkg.getDirectory();
}

const std::string&
PackageReader::getName() const
{
    return m_pkg.getName();
}

bool
PackageReader::isPostEnsmallening() const
{
    return m_pkg.isPostEnsmallening();
}

void
PackageReader::lsDir(const LotusPath& internalPath) const
{
    PackageSplitReader splitH = m_pkg[PackageTrioType::H];
    splitH->readToc();
    return splitH->lsDir(internalPath);
}

PackageReader
PackagesReader::getPackage(std::string name)
{
    Package<CachePairReader>& pkg = m_packgesDir[name];
    return PackageReader(pkg);
}