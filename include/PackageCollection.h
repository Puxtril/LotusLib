#pragma once

#include "Package.h"

#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string>

namespace LotusLib
{
	//! \brief Provides templated access to Packages
	//!
	//! H.Texture.toc
	//!   \------- 
	//!      |
	//! You are here
	template <class T>
	class PackageCollection
	{
		std::filesystem::path m_packageDir;
		bool m_isPostEnsmallening;
		std::unordered_map<std::string, int> m_pkgMap;
		std::vector<Package<T>> m_pkgs;
		Logger& m_log;

	public:
		PackageCollection(std::filesystem::path pkgDir, bool isPostEnsmallening)
			: m_packageDir(pkgDir), m_isPostEnsmallening(isPostEnsmallening), m_log(Logger::getInstance())
		{
			static_assert(std::is_base_of<CachePair, T>::value, "PackageCollection type must be child class of CachePair");
			_loadPackages();
		}

		PackageCollection(const PackageCollection&) = delete;
		PackageCollection& operator=(PackageCollection&) = delete;
		PackageCollection(PackageCollection&&) = default;
		PackageCollection& operator=(PackageCollection&&) = default;

		typename std::vector<Package<T>>::iterator begin() { return m_pkgs.begin(); }
		typename std::vector<Package<T>>::iterator end() { return m_pkgs.end(); }
		typename std::vector<Package<T>>::const_iterator begin() const { return m_pkgs.begin(); }
		typename std::vector<Package<T>>::const_iterator end() const { return m_pkgs.end(); }

		const Package<T>&
		operator[](const std::string& pkgName) const
		{
			return m_pkgs[m_pkgMap.at(pkgName)];
		}

		Package<T>&
		operator[](const std::string& pkgName)
		{
			return m_pkgs[m_pkgMap.at(pkgName)];
		}

		const std::filesystem::path& getPath() const { return m_packageDir; }
		bool isPostEnsmallening() const { return m_isPostEnsmallening; }

	protected:
		void _loadPackages()
		{
			for (auto& f : std::filesystem::directory_iterator(m_packageDir))
			{
				std::string filename = f.path().filename().string();
				size_t start = filename.find('.');
				size_t end = filename.find('.', start + 1);
		
				if (start == std::string::npos || end == std::string::npos)
					continue;
		
				std::string pkgName = filename.substr(start + 1, end - 2);
				if (m_pkgMap.find(pkgName) == m_pkgMap.end())
					_loadPackage(pkgName);
			}
		
			std::string pkgList = "";
			for (const auto& x : m_pkgs)
				pkgList += x.getName() + " ";
			m_log.info(spdlog::fmt_lib::format("Loaded {} packages: {}", m_pkgs.size(), pkgList));
		}

		virtual void _loadPackage(std::string pkgName)
		{
			// Because Package does not have a copy constructor
			// https://stackoverflow.com/a/27699086
			m_pkgMap.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(pkgName),
				std::forward_as_tuple(static_cast<int>(m_pkgs.size()))
			);
			m_pkgs.emplace_back(m_packageDir, pkgName, m_isPostEnsmallening);
		} 
	};
}
