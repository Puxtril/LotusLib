#pragma once

#include "CachePair.h"
#include "Logger.h"

#include "CachePair.h"
#include "CommonHeader.h"

#include <filesystem>
#include <string>
#include <memory>
#include <array>
#include <optional>

namespace LotusLib
{
	enum class PackageTrioType : char {
		H = 0,
		F = 1,
		B = 2
	};

	//! \brief Provided a templated container for CachePairs
	//!
	//! H.Texture.toc
	//! \- 
	//! |
	//! You are here
	class Package
	{
		Logger& m_log;
		std::filesystem::path m_directory;
		std::string m_name;
		bool m_isPostEnsmallening;
		std::array<std::optional<CachePair>, 3> m_pkgs;

	public:
		Package(std::filesystem::path pkgDir, std::string pkgName, bool isPostEnsmallening);

		std::array<std::optional<CachePair>, 3>::iterator begin();
		std::array<std::optional<CachePair>, 3>::iterator end();
		std::array<std::optional<CachePair>, 3>::const_iterator begin() const;
		std::array<std::optional<CachePair>, 3>::const_iterator end() const;
		
		const std::optional<CachePair> operator[](PackageTrioType trioType) const;
		std::optional<CachePair> operator[](PackageTrioType trioType);

		const std::filesystem::path& getDirectory() const;
		const std::string& getName() const;
		bool isPostEnsmallening() const;
		
	private:
		void loadPkgPairs();
		std::tuple<std::filesystem::path, std::filesystem::path> getPairPath(PackageTrioType trioType);
	};
}
