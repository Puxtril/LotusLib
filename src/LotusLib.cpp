#include "LotusLib.h"

using namespace LotusLib;

CommonHeader
PackageReader::getCommonHeader(LotusPath internalPath)
{
    LotusLib::CachePair* split = m_pkg->getPair(PackageTrioType::H);
    split->readToc();
    FileRef fileRef = split->getFileEntry(internalPath);
    return getCommonHeader(fileRef);
}

CommonHeader
PackageReader::getCommonHeader(const FileNode* fileRef)
{
    LotusLib::CachePair* splitH = m_pkg->getPair(PackageTrioType::H);
    splitH->readToc();
    std::vector<uint8_t> dataHeader = splitH->getDataAndDecompress(fileRef);
    auto reader = BinaryReader::BinaryReaderBuffered(std::move(dataHeader));
    CommonHeader ch = commonHeaderRead(reader);
    return ch;
}

int
PackageReader::getFileFormat(LotusPath internalPath)
{
    LotusLib::CachePair* split = m_pkg->getPair(PackageTrioType::H);
    split->readToc();
    FileRef fileRef = split->getFileEntry(internalPath);
    return getFileFormat(fileRef);
}

int
PackageReader::getFileFormat(const FileNode* fileRef)
{
    LotusLib::CachePair* splitH = m_pkg->getPair(PackageTrioType::H);
    splitH->readToc();
    std::vector<uint8_t> dataHeader = splitH->getDataAndDecompress(fileRef);
    auto reader = BinaryReader::BinaryReaderBuffered(std::move(dataHeader));
    uint32_t format = commonHeaderReadFormat(reader);
    return (int)format;
}

FileEntry
PackageReader::getFile(LotusPath internalPath)
{
    LotusLib::CachePair* split = m_pkg->getPair(PackageTrioType::H);
    split->readToc();

    try
    {
        FileRef fileRef = split->getFileEntry(internalPath);
        return getFile(fileRef);
    }
    catch (InternalEntryNotFound&)
    {
        return getFileOnlyPackagesBin(internalPath);
    }
    
}

FileEntry
PackageReader::getFile(LotusPath internalPath, int fileEntryReaderFlags)
{
    LotusLib::CachePair* split = m_pkg->getPair(PackageTrioType::H);
    split->readToc();
    
    try
    {
        FileRef fileRef = split->getFileEntry(internalPath);
        return getFile(fileRef, fileEntryReaderFlags);
    }
    catch (InternalEntryNotFound&)
    {
        if (fileEntryReaderFlags & READ_EXTRA_ATTRIBUTES)
            return getFileOnlyPackagesBin(internalPath);
    }

    FileEntry entry;
    entry.internalPath = internalPath;
    return entry;
}

FileEntry
PackageReader::getFile(const FileNode* fileRef)
{
    return getFile(fileRef, -1);
}

FileEntry
PackageReader::getFile(const FileNode* fileRef, int fileEntryReaderFlags)
{
    FileEntry entry;

    entry.internalPath = fileRef->getFullPath();
    entry.srcPkgName = m_pkg->getName();

    LotusLib::CachePair* splitH = m_pkg->getPair(PackageTrioType::H);
    splitH->readToc();
    entry.metadata = splitH->getFileEntry(entry.internalPath);

    if (fileEntryReaderFlags & READ_H_CACHE || fileEntryReaderFlags & READ_COMMON_HEADER)
    {
        std::vector<uint8_t> dataHeader = splitH->getDataAndDecompress(fileRef);
        entry.headerData = BinaryReader::BinaryReaderBuffered(std::move(dataHeader));
    }

    if (fileEntryReaderFlags & READ_COMMON_HEADER)
    {
        entry.commonHeader = commonHeaderRead(entry.headerData);
        size_t commonHeaderLen = commonHeaderFindLen(entry.headerData);
        entry.headerData.seek(commonHeaderLen, std::ios::beg);
    }

    if (fileEntryReaderFlags & READ_B_CACHE)
    {
        LotusLib::CachePair* splitB = m_pkg->getPair(PackageTrioType::B);
        if (splitB)
        {
            splitB->readToc();
            try
            {
                FileRef entryB = splitB->getFileEntry(entry.internalPath);
                std::vector<uint8_t> dataB = splitB->getDataAndDecompress(entryB);
                entry.bData = BinaryReader::BinaryReaderBuffered(std::move(dataB));
            }
            catch (InternalEntryNotFound&) { }
        }
    }

    if (fileEntryReaderFlags & READ_F_CACHE)
    {
        LotusLib::CachePair* splitF = m_pkg->getPair(PackageTrioType::F);
        if (splitF)
        {
            splitF->readToc();
            try
            {
                FileRef entryF = splitF->getFileEntry(entry.internalPath);
                std::vector<uint8_t> dataF = splitF->getDataAndDecompress(entryF);
                entry.fData = BinaryReader::BinaryReaderBuffered(std::move(dataF));
            }
            catch (InternalEntryNotFound&) { }
        }
    }

    if (fileEntryReaderFlags & READ_EXTRA_ATTRIBUTES)
    {
        entry.extra.parent = m_packagesBin->getParent(entry.internalPath);
        entry.extra.attributes = m_packagesBin->getParameters(entry.internalPath);
    }

    return entry;
}

const FileNode*
PackageReader::getFileNode(LotusPath internalPath, PackageTrioType trioType)
{
    LotusLib::CachePair* split = m_pkg->getPair(trioType);
    if (split == nullptr)
        return nullptr;
    split->readToc();
    return split->getFileEntry(internalPath);
}

const DirNode*
PackageReader::getDirNode(LotusPath internalPath, PackageTrioType trioType)
{
    LotusLib::CachePair* split = m_pkg->getPair(trioType);
    if (split == nullptr)
        return nullptr;
    split->readToc();
    return split->getDirEntry(internalPath);
}

DirectoryTree::ConstFileIterator
PackageReader::begin() const
{
    LotusLib::CachePair* splitH = m_pkg->getPair(PackageTrioType::H);
    splitH->readToc();
    return splitH->begin();
}

DirectoryTree::ConstFileIterator
PackageReader::end() const
{
    LotusLib::CachePair* splitH = m_pkg->getPair(PackageTrioType::H);
    splitH->readToc();
    return splitH->end();
}

DirectoryTree::FileIteratorTree
PackageReader::getIter(LotusLib::LotusPath startingPath, PackageTrioType trioType) const
{
    LotusLib::CachePair* split = m_pkg->getPair(trioType);
    if (split == nullptr)
    {
        LotusLib::CachePair* splitH = m_pkg->getPair(PackageTrioType::H);
        splitH->readToc();
        return splitH->getIter();
    }
    split->readToc();
    return split->getIter(startingPath);
}

DirectoryTree::FileIteratorTree
PackageReader::getIter(PackageTrioType trioType) const
{
    LotusLib::CachePair* split = m_pkg->getPair(trioType);
    if (split == nullptr)
    {
        LotusLib::CachePair* splitH = m_pkg->getPair(PackageTrioType::H);
        splitH->readToc();
        return splitH->getIter();
    }
    split->readToc();
    return split->getIter();
}

const std::filesystem::path&
PackageReader::getDirectory() const
{
    return m_pkg->getDirectory();
}

const std::string&
PackageReader::getName() const
{
    return m_pkg->getName();
}

bool
PackageReader::isPostEnsmallening() const
{
    return m_pkg->isPostEnsmallening();
}

void
PackageReader::lsDir(const LotusPath& internalPath) const
{
    LotusLib::CachePair* splitH = m_pkg->getPair(PackageTrioType::H);
    splitH->readToc();
    return splitH->lsDir(internalPath);
}

FileEntry
PackageReader::getFileOnlyPackagesBin(LotusLib::LotusPath internalpath)
{
    FileEntry entry;

    entry.internalPath = internalpath;

    entry.extra.parent = m_packagesBin->getParent(entry.internalPath);
    entry.extra.attributes = m_packagesBin->getParameters(entry.internalPath);

    return entry;
}

std::optional<PackageReader>
PackagesReader::getPackage(const std::string& name)
{
    Package* pkg = m_packgesDir.getPackage(name);
    if (pkg == nullptr)
        return std::nullopt;
    return PackageReader(pkg, &m_packagesBin);
}

void
PackagesReader::initilizePackagesBin()
{
    if (!m_packagesBin.isInitilized())
    {
        logInfo("Reading Packages.bin");

        LotusLib::Package* pkgMisc = m_packgesDir.getPackage("Misc");
        LotusLib::CachePair* pair = pkgMisc->getPair(LotusLib::PackageTrioType::H);
        pair->readToc();
        std::vector<uint8_t> packagesRaw = pair->getDataAndDecompress("Packages.bin");
        BinaryReader::BinaryReaderBuffered reader(std::move(packagesRaw));
        m_packagesBin.initilize(reader);
    }
}