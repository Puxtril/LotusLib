#pragma once

#include "BinaryReader/Buffered.h"
#include "LotusLib/CommonHeader.h"
#include "LotusLib/Enums.h"
#include "LotusLib/PackageSplit.h"
#include "LotusLib/Logger.h"
#include "LotusLib/Utils.h"

#include <array>
#include <filesystem>
#include <optional>
#include <string>

namespace LotusLib
{
	struct FileEntry
	{
		CommonHeader commonHeader;
		BinaryReader::Buffered header;
		BinaryReader::Buffered body;
		BinaryReader::Buffered footer;
	};

	class Package
	{
		std::filesystem::path m_directory;
		std::string m_name;
		Game m_game;
		PackageCategory m_category;
		std::array<std::optional<PackageSplit>, 3> m_pkgs;

	public:
		Package(std::filesystem::path pkgDir, std::string pkgName, Game game);
		
		bool hasSplit(PkgSplitType split) const;
		PackageSplit getSplit(PkgSplitType pkgSplit) const;

		// Returns FileNode inside HEADER PkgSplit
		std::vector<FileNode>::const_iterator begin() const;
		std::vector<FileNode>::const_iterator end() const;
		TOCTreeIterator getIter(const std::string& path) const;
		TOCTreeIterator getIter() const;

		const std::filesystem::path& getDirectory() const;
		const std::string& getName() const;
		Game getGame() const;
		PackageCategory getPkgCategory() const;

		// FileNode MUST be from HEADER PkgSplit
		FileEntry getFileEntry(const std::string& internalPath) const;
		FileEntry getFileEntry(const FileNode& entry) const;

		bool fileExists(PkgSplitType split, const std::string& internalPath) const;
		bool fileExists(PkgSplitType split, const FileNode& fileNode) const;
		bool fileExists(const std::string& internalPath) const;
		bool fileExists(const FileNode& fileNode) const;
		bool dirExists(PkgSplitType split, const std::string& internalPath) const;
		bool dirExists(PkgSplitType split, const DirNode& dirNode) const;
		bool dirExists(const std::string& internalPath) const;
		bool dirExists(const DirNode& dirNode) const;

		const FileNode& getFileNode(PkgSplitType split, const FileNode& fileNode) const;
		const DirNode& getDirNode(PkgSplitType split, const DirNode& dirNode) const;

		const FileNode& getFileNode(PkgSplitType split, const std::string& internalPath) const;
		const DirNode& getDirNode(PkgSplitType split, const std::string& internalPath) const;

		size_t dirCount() const;
		size_t dirCount(PkgSplitType split) const;
		size_t fileCount() const;
		size_t fileCount(PkgSplitType split) const;
		size_t fileDupeCount() const;
		size_t fileDupeCount(PkgSplitType split) const;

		std::vector<uint8_t> getFileUncompressed(PkgSplitType split, const std::string& internalPath) const;
		std::vector<uint8_t> getFileUncompressed(PkgSplitType split, const FileNode& fileNode) const;
		std::vector<uint8_t> getFile(PkgSplitType split, const std::string& internalPath) const;
		std::vector<uint8_t> getFile(PkgSplitType split, const FileNode& fileNode) const;

		// Only accepts FileNode structs from HEADER PkgSplit
		CommonHeader readCommonHeader(const FileNode& entry) const;
		uint32_t readCommonHeaderFormat(const FileNode& entry) const;
		
	private:
		void loadPkgSplits();
		std::tuple<std::filesystem::path, std::filesystem::path> getSplitPath(PkgSplitType pkgSplit);
	};
}
