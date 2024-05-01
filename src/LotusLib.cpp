#include "LotusLib.h"

using namespace LotusLib;

//////////////////////////////////////////////////////////////////
// FileMeta

FileMeta::FileMeta(const FileEntries::FileNode* fileNode)
: fileNode(fileNode)
{ }

FileMeta::FileMeta()
: fileNode(nullptr)
{ }

bool
FileMeta::isEmpty() const
{
    return fileNode == nullptr;
}

const std::string&
FileMeta::getName() const
{
    return fileNode->getName();
}

int32_t
FileMeta::getLen() const
{
    return fileNode->getLen();
}

int32_t
FileMeta::getCompLen() const
{
    return fileNode->getCompLen();
}

int64_t
FileMeta::getOffset() const
{
    return fileNode->getOffset();
}

int64_t
FileMeta::getTimeStamp() const
{
    return fileNode->getTimeStamp();
}

std::string
FileMeta::getFullPath() const
{
    return fileNode->getFullPath();
}

//////////////////////////////////////////////////////////////////
// DirMeta

DirMeta::DirMeta(const FileEntries::DirNode* dirNode)
  : dirNode(dirNode)
{ }

DirMeta::DirMeta()
  : dirNode(nullptr)
{ }

bool
DirMeta::isEmpty() const
{
    return dirNode == nullptr;
}

const std::string&
DirMeta::getName() const
{
    return dirNode->getName();
}

const DirMeta
DirMeta::getParent() const
{
    return DirMeta(dirNode->getParent());
}

int
DirMeta::getTocOffset() const
{
    return dirNode->getTocOffset();
}

size_t
DirMeta::getDirCount() const
{
    return dirNode->getDirCount();
}

size_t
DirMeta::getFileCount() const
{
    return dirNode->getFileCount();
}

std::string
DirMeta::getFullPath() const
{
    return dirNode->getFullPath();
}

const DirMeta
DirMeta::getChildDir(int index) const
{
    return DirMeta(dirNode->getChildDir(index));
}

const DirMeta
DirMeta::getChildDir(const std::string& name) const
{
    return DirMeta(dirNode->getChildDir(name));
}

const FileMeta
DirMeta::getChildFile(int index) const
{
    return FileMeta(dirNode->getChildFile(index));
}

const FileMeta
DirMeta::getChildFile(const std::string& name) const
{
    return FileMeta(dirNode->getChildFile(name));
}

//////////////////////////////////////////////////////////////////
// Package Reader

CommonHeader
PackageReader::getCommonHeader(LotusPath internalPath)
{
    PackageSplitReader split = m_pkg[PackageTrioType::H];
    split->readToc();
    FileRef fileRef = split->getFileEntry(internalPath);
    return getCommonHeader(*fileRef);
}

CommonHeader
PackageReader::getCommonHeader(const FileNode& fileRef)
{
    PackageSplitReader splitH = m_pkg[PackageTrioType::H];
    splitH->readToc();
    std::vector<uint8_t> dataHeader = splitH->getDataAndDecompress(&fileRef);
    auto reader = BinaryReader::BinaryReaderBuffered(std::move(dataHeader));
    CommonHeader ch = CHRead(reader);
    return ch;
}

FileEntry
PackageReader::getFile(LotusPath internalPath)
{
    PackageSplitReader split = m_pkg[PackageTrioType::H];
    split->readToc();

    try
    {
        FileRef fileRef = split->getFileEntry(internalPath);
        return getFile(fileRef);
    }
    catch (std::runtime_error&)
    {
        return getFileOnlyPackagesBin(internalPath);
    }
    
}

FileEntry
PackageReader::getFile(LotusPath internalPath, int fileEntryReaderFlags)
{
    PackageSplitReader split = m_pkg[PackageTrioType::H];
    split->readToc();
    
    try
    {
        FileRef fileRef = split->getFileEntry(internalPath);
        return getFile(fileRef, fileEntryReaderFlags);
    }
    catch (std::runtime_error&)
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
    PackageSplitReader splitH = m_pkg[PackageTrioType::H];
    splitH->readToc();
    entry.metadata = FileMeta(splitH->getFileEntry(entry.internalPath));

    if (fileEntryReaderFlags & READ_COMMON_HEADER)
    {
        std::vector<uint8_t> dataHeader = splitH->getDataAndDecompress(fileRef);
        entry.headerData = BinaryReader::BinaryReaderBuffered(std::move(dataHeader));
        entry.commonHeader = CHRead(entry.headerData);
    }
    else if (fileEntryReaderFlags & READ_H_CACHE)
    {
        std::vector<uint8_t> dataHeader = splitH->getDataAndDecompress(fileRef);
        entry.headerData = BinaryReader::BinaryReaderBuffered(std::move(dataHeader));
        size_t chSize = CHFindLen(entry.headerData);
        entry.headerData.seek(chSize, std::ios::beg);
    }

    if (fileEntryReaderFlags & READ_B_CACHE)
    {
        PackageSplitReader splitB = m_pkg[PackageTrioType::B];
        if (splitB)
        {
            splitB->readToc();
            try
            {
                FileRef entryB = splitB->getFileEntry(entry.internalPath);
                std::vector<uint8_t> dataB = splitB->getDataAndDecompress(entryB);
                entry.bData = BinaryReader::BinaryReaderBuffered(std::move(dataB));
            }
            catch (std::runtime_error&) { }
        }
    }

    if (fileEntryReaderFlags & READ_F_CACHE)
    {
        PackageSplitReader splitF = m_pkg[PackageTrioType::F];
        if (splitF)
        {
            splitF->readToc();
            try
            {
                FileRef entryF = splitF->getFileEntry(entry.internalPath);
                std::vector<uint8_t> dataF = splitF->getDataAndDecompress(entryF);
                entry.fData = BinaryReader::BinaryReaderBuffered(std::move(dataF));
            }
            catch (std::runtime_error&) { }
        }
    }

    if (fileEntryReaderFlags & READ_EXTRA_ATTRIBUTES)
    {
        entry.extra.parent = m_packagesBin->getParent(entry.internalPath);
        entry.extra.attributes = m_packagesBin->getParameters(entry.internalPath);
    }

    return entry;
}

FileEntry
PackageReader::getFile(const FileMeta& fileRef)
{
    return getFile(fileRef.fileNode);
}

FileEntry
PackageReader::getFile(const FileMeta& fileRef, int fileEntryReaderFlags)
{
    return getFile(fileRef.fileNode, fileEntryReaderFlags);
}

FileMeta
PackageReader::getFileMeta(LotusPath internalPath)
{
    PackageSplitReader split = m_pkg[PackageTrioType::H];
    split->readToc();
    LotusLib::FileEntries::FileNode* fileRef = split->getFileEntry(internalPath);
    return FileMeta(fileRef);
}

DirMeta
PackageReader::getDirMeta(LotusPath internalPath)
{
    PackageSplitReader split = m_pkg[PackageTrioType::H];
    split->readToc();
    LotusLib::FileEntries::DirNode* dirRef = split->getDirEntry(internalPath);
    return DirMeta(dirRef);
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

DirectoryTree::FileIteratorTree
PackageReader::getIter(LotusLib::LotusPath startingPath) const
{
    PackageSplitReader splitH = m_pkg[PackageTrioType::H];
    splitH->readToc();
    return splitH->getIter(startingPath);
}

DirectoryTree::FileIteratorTree
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

FileEntry
PackageReader::getFileOnlyPackagesBin(LotusLib::LotusPath internalpath)
{
    FileEntry entry;

    entry.internalPath = internalpath;

    entry.extra.parent = m_packagesBin->getParent(entry.internalPath);
    entry.extra.attributes = m_packagesBin->getParameters(entry.internalPath);

    return entry;
}

PackageReader
PackagesReader::getPackage(std::string name)
{
    Package& pkg = m_packgesDir[name];
    return PackageReader(pkg, &m_packagesBin);
}

void
PackagesReader::initilizePackagesBin()
{
    if (!m_packagesBin.isInitilized())
    {
        Logger::getInstance().info("Reading Packages.bin");

        LotusLib::Package& pkgMisc = m_packgesDir["Misc"];
        std::optional<LotusLib::CachePair> pair = pkgMisc[LotusLib::PackageTrioType::H];
        pair->readToc();
        std::vector<uint8_t> packagesRaw = pair->getDataAndDecompress("Packages.bin");
        BinaryReader::BinaryReaderBuffered reader(std::move(packagesRaw));
        m_packagesBin.initilize(reader);
    }
}