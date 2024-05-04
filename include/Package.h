#pragma once

#include "CachePair.h"
#include "LotusLibLogger.h"

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

	class Package
	{
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
		
		// Returns nullptr if trioType doesn't exist
		const CachePair* getPair(PackageTrioType trioType) const;
		CachePair* getPair(PackageTrioType trioType);

		const std::filesystem::path& getDirectory() const;
		const std::string& getName() const;
		bool isPostEnsmallening() const;
		
	private:
		void loadPkgPairs();
		std::tuple<std::filesystem::path, std::filesystem::path> getPairPath(PackageTrioType trioType);
	};
}
