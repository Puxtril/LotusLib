#include "LotusLib/PackageSplit.h"

using namespace LotusLib;

Impl::PackageSplitState::PackageSplitState(std::filesystem::path tocPath, std::filesystem::path cachePath)
	: tocTree(tocPath)
{
	if (!std::filesystem::exists(cachePath))
	{
		cacheExists = false;		
	}
	else
	{
		cacheExists = true;
		cacheReader = std::ifstream(cachePath, std::ios::in | std::ios::binary);
	}
}

PackageSplit::PackageSplit(std::filesystem::path tocPath, std::filesystem::path cachePath, Game game, PkgSplitType pkgSplit)
	: m_game(game),
	m_pkgSplit(pkgSplit),
	m_tocPath(tocPath),
	m_cachePath(cachePath),
	m_state(std::make_shared<Impl::PackageSplitState>(tocPath, cachePath)),
	m_compressScratch(std::make_shared<Impl::CompressionScratch>()),
	m_commonheaderScratch(std::make_shared<Impl::CompressionScratch>())
{
	if (!_isTOCValid())
		throw LotusException("PackageSplit is not valid: " + m_tocPath.string());
}

Game
PackageSplit::getGame() const
{
	return m_game;
}

const std::filesystem::path&
PackageSplit::getTocPath() const
{
	return m_tocPath;
}

const std::filesystem::path&
PackageSplit::getCachePath() const
{
	return m_cachePath;
}

std::vector<FileNode>::const_iterator
PackageSplit::begin() const
{
	m_state->tocTree.initialize();
	return m_state->tocTree.begin();
}

std::vector<FileNode>::const_iterator
PackageSplit::end() const
{
	m_state->tocTree.initialize();
	return m_state->tocTree.end();
}

TOCTreeIterator
PackageSplit::getIter(const std::string& path) const
{
	m_state->tocTree.initialize();
	return m_state->tocTree.getIter(path);
}

TOCTreeIterator
PackageSplit::getIter() const
{
	m_state->tocTree.initialize();
	return m_state->tocTree.getIter();
}

bool
PackageSplit::fileExists(const std::string& internalPath) const
{
	return m_state->tocTree.fileExists(internalPath);
}

bool
PackageSplit::dirExists(const std::string& internalPath) const
{
	return m_state->tocTree.dirExists(internalPath);
}

bool
PackageSplit::fileExists(const FileNode& fileNode) const
{
	return m_state->tocTree.fileExists(fileNode);
}

bool
PackageSplit::dirExists(const DirNode& dirNode) const
{
	return m_state->tocTree.dirExists(dirNode);
}

const FileNode&
PackageSplit::getFileNode(const FileNode& fileNode) const
{
	return m_state->tocTree.getFileNode(fileNode);
}

const DirNode&
PackageSplit::getDirNode(const DirNode& dirNode) const
{
	return m_state->tocTree.getDirNode(dirNode);
}

const FileNode&
PackageSplit::getFileNode(const std::string& internalPath) const
{
	m_state->tocTree.initialize();
	return m_state->tocTree.getFileNode(internalPath);
}

const DirNode&
PackageSplit::getDirNode(const std::string& internalPath) const
{
	m_state->tocTree.initialize();
	return m_state->tocTree.getDirNode(internalPath);
}

size_t
PackageSplit::dirCount() const
{
	m_state->tocTree.initialize();
	return m_state->tocTree.dirCount();
}

size_t
PackageSplit::fileCount() const
{
	m_state->tocTree.initialize();
	return m_state->tocTree.fileCount();
}

size_t
PackageSplit::fileDupeCount() const
{
	m_state->tocTree.initialize();
	return m_state->tocTree.fileDupeCount();
}

std::vector<uint8_t>
PackageSplit::getFileUncompressed(const std::string& internalPath) const
{
	m_state->tocTree.initialize();
	const FileNode& entry = getFileNode(internalPath);
	return getFileUncompressed(entry);
}

std::vector<uint8_t>
PackageSplit::getFileUncompressed(const FileNode& entry) const
{
	if (!m_state->cacheExists)
		throw LotusException("Cannot get data, cache file missing: " + m_cachePath.string());

	m_state->tocTree.initialize();

	m_state->cacheReader.seekg(entry.cacheOffset, std::ios_base::beg);
	std::vector<uint8_t> data(entry.compLen);
	m_state->cacheReader.read((char*)data.data(), entry.compLen);
	return data;
}

std::vector<uint8_t>
PackageSplit::getFile(const std::string& internalPath) const
{
	m_state->tocTree.initialize();
	const FileNode& entry = getFileNode(internalPath);
	return getFile(entry);
}

std::vector<uint8_t>
PackageSplit::getFile(const FileNode& entry) const
{
	if (!m_state->cacheExists)
		throw LotusException("Cannot get data, cache file missing: " + m_cachePath.string());

	m_state->tocTree.initialize();
	
	if (entry.compLen == entry.len)
		return getFileUncompressed(entry);

	std::vector<uint8_t> decompressedData(entry.len);
	decompress(entry, decompressedData.data());
	return decompressedData;
}

CommonHeader
PackageSplit::readCommonHeader(const FileNode& entry)
{
	if (m_pkgSplit != PkgSplitType::HEADER)
		throw LotusException("Can only read CommonHeader inside H");
	if (!m_state->cacheExists)
		throw LotusException("Cannot get data, cache file missing: " + m_cachePath.string());

	m_state->tocTree.initialize();

	if (entry.len > static_cast<int32_t>(m_commonheaderScratch->buf.size()))
		m_commonheaderScratch->buf.resize(entry.len);

	decompress(entry, m_commonheaderScratch->buf.data());

	auto reader = BinaryReader::BinaryReaderSlice(m_commonheaderScratch->buf.data(), entry.len);
	return commonHeaderRead(reader, m_game);
}

uint32_t
PackageSplit::readCommonHeaderFormat(const FileNode& entry)
{
	if (m_pkgSplit != PkgSplitType::HEADER)
		throw LotusException("Can only read CommonHeader inside H");
	if (!m_state->cacheExists)
		throw LotusException("Cannot get data, cache file missing: " + m_cachePath.string());

	m_state->tocTree.initialize();

	if (entry.len > static_cast<int32_t>(m_commonheaderScratch->buf.size()))
		m_commonheaderScratch->buf.resize(entry.len);

	decompress(entry, m_commonheaderScratch->buf.data());

	auto reader = BinaryReader::BinaryReaderSlice(m_commonheaderScratch->buf.data(), entry.len);
	return commonHeaderReadFormat(reader, m_game);
}

void
PackageSplit::decompress(const FileNode& entry, uint8_t* outData) const
{
	if (entry.compLen == entry.len)
	{
		m_state->cacheReader.seekg(entry.cacheOffset, std::ios_base::beg);
		m_state->cacheReader.read((char*)outData, entry.compLen);
		return;
	}

	std::lock_guard<std::mutex> guard(m_state->decompressMutex);

	switch(m_game)
	{
		case Game::DARKNESSII:
		case Game::STARTREK:
			Impl::Compression::decompressEE(m_compressScratch.get(), entry, m_state->cacheReader, outData);
			break;
		case Game::WARFRAME_PE:
			Impl::Compression::decompressWarframePre(m_compressScratch.get(), entry, m_state->cacheReader, outData);
			break;
		case Game::WARFRAME:
		case Game::SOULFRAME:
			Impl::Compression::decompressWarframePost(m_compressScratch.get(), entry, m_state->cacheReader, outData);
			break;
		default:
			throw LotusException("Cannot decompress " + gameToString(m_game));
	}
}

bool
PackageSplit::_isTOCValid() const
{
	std::ifstream tocReader(m_tocPath, std::ios_base::in | std::ios_base::binary);

	unsigned int magicNumber;
	unsigned int archiveVersion;

	tocReader.read(reinterpret_cast<char*>(&magicNumber), 4);
	tocReader.read(reinterpret_cast<char*>(&archiveVersion), 4);

	if (magicNumber != m_magicNumber)
		return false;
	
	if (archiveVersion != m_archiveVersion1 && archiveVersion != m_archiveVersion2)
		return false;

	return true;
}
