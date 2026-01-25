#pragma once

#include "LotusLib/Package.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace LotusLib
{
	class PackageCollection
	{
		std::filesystem::path m_packageDir;
		Game m_game;
		std::unordered_map<std::string, int> m_pkgMap;
		std::vector<Package> m_pkgs;

	public:
		PackageCollection(std::filesystem::path pkgDir, Game game);

		std::vector<Package>::iterator begin();
		std::vector<Package>::iterator end();
		std::vector<Package>::const_iterator begin() const;
		std::vector<Package>::const_iterator end() const;

		bool hasPackage(const std::string& pkgName) const;
		Package getPackage(const std::string& pkgName) const;

		const std::filesystem::path& getPath() const;
		Game getGame() const;

		const FileNode& getFileNode(const std::string& pkgName, PkgSplitType split, const std::string& internalPath) const;
		const DirNode& getDirNode(const std::string& pkgName, PkgSplitType split, const std::string& internalPath) const;

		size_t dirCount() const;
		size_t dirCount(const std::string& pkgName) const;
		size_t dirCount(const std::string& pkgName, PkgSplitType split) const;
		size_t fileCount() const;
		size_t fileCount(const std::string& pkgName) const;
		size_t fileCount(const std::string& pkgName, PkgSplitType split) const;
		size_t fileDupeCount() const;
		size_t fileDupeCount(const std::string& pkgName) const;
		size_t fileDupeCount(const std::string& pkgName, PkgSplitType split) const;

		std::vector<uint8_t> getFileUncompressed(const std::string& pkgName, PkgSplitType split, const std::string& internalPath) const;
		std::vector<uint8_t> getFile(const std::string& pkgName, PkgSplitType split, const std::string& internalPath) const;

	protected:
		void loadPackages();
		void loadPackage(std::string pkgName);
	};
}
