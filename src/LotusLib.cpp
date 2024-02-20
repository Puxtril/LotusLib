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