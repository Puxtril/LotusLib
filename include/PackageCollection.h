#pragma once

#include "Package.h"

#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string>

namespace LotusLib
{
	class PackageCollection
	{
		std::filesystem::path m_packageDir;
		bool m_isPostEnsmallening;
		std::unordered_map<std::string, int> m_pkgMap;
		std::vector<Package> m_pkgs;
		Logger& m_log;

	public:
		PackageCollection();
		PackageCollection(std::filesystem::path pkgDir, bool isPostEnsmallening);

		void setData(std::filesystem::path pkgDir, bool isPostEnsmallening);

		std::vector<Package>::iterator begin();
		std::vector<Package>::iterator end();
		std::vector<Package>::const_iterator begin() const;
		std::vector<Package>::const_iterator end() const;

		const Package& operator[](const std::string& pkgName) const;
		Package& operator[](const std::string& pkgName);

		const std::filesystem::path& getPath() const;
		bool isPostEnsmallening() const;

	protected:
		void loadPackages();
		void loadPackage(std::string pkgName);
	};
}
