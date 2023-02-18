#pragma once

#include "CachePair.h"
#include "Logger.h"

//For specializations
#include "CachePairMeta.h"
#include "CachePairReader.h"
#include "CommonHeader.h"

#include <filesystem>
#include <string>
#include <memory>
#include <array>
#include <type_traits>

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
	template <class T>
	class Package
	{
		Logger& m_log;
		std::filesystem::path m_directory;
		std::string m_name;
		bool m_isPostEnsmallening;
		mutable std::array<std::shared_ptr<T>, 3> m_pkgs;

	public:
		////////////////////////////////////////////////////
		// Constructors

		Package(std::filesystem::path pkgDir, std::string pkgName, bool isPostEnsmallening)
			: m_log(Logger::getInstance()), m_directory(pkgDir), m_name(pkgName), m_isPostEnsmallening(isPostEnsmallening), m_pkgs()
		{
			static_assert(std::is_base_of<CachePair, T>::value, "Package type must be child class of CachePair");
			_loadPkgPairs();
		}

		Package(const Package&) = delete;
		Package& operator=(Package&) = delete;
		Package(Package&&) = default;
		Package& operator=(Package&&) = default;

		////////////////////////////////////////////////////
		// Iterators

		typename
		std::array<std::shared_ptr<T>, 3>::iterator
		begin()
		{
			return m_pkgs.begin();
		}
		
		typename
		std::array<std::shared_ptr<T>, 3>::iterator
		end()
		{
			return m_pkgs.end();
		}
		
		typename
		std::array<std::shared_ptr<T>, 3>::const_iterator
		begin() const
		{
			return m_pkgs.begin();
		}
		
		typename
		std::array<std::shared_ptr<T>, 3>::const_iterator
		end() const
		{
			return m_pkgs.end();
		}

		////////////////////////////////////////////////////
		// Index Operators
		
		const typename std::shared_ptr<T> operator[](PackageTrioType trioType) const
		{
			return m_pkgs[(int)trioType];
		}

		typename std::shared_ptr<T> operator[](PackageTrioType trioType)
		{
			return m_pkgs[(int)trioType];
		}
		
		////////////////////////////////////////////////////
		// Get-ers

		const std::filesystem::path&
		getDirectory() const
		{
			return m_directory;
		}
		
		const std::string&
		getName() const
		{
			return m_name;
		}
		
		bool
		isPostEnsmallening() const
		{
			return m_isPostEnsmallening;
		}
		
	private:
		////////////////////////////////////////////////////
		// Helper Methods
		
		void
		_loadPkgPairs()
		{
			for (int i = 0; i < 3; i++)
			{
				auto pair = getPairPath((PackageTrioType)i);
				if (std::filesystem::exists(std::get<0>(pair)) && std::filesystem::exists(std::get<1>(pair)))
				{
					std::shared_ptr<T> x = std::make_shared<T>(std::get<0>(pair), std::get<1>(pair), m_isPostEnsmallening);
					m_pkgs[i] = x;
				}
				else
				{
					m_log.debug(spdlog::fmt_lib::format("Package does not exist: {}", std::get<0>(pair).stem().string()));
				}
			}
		}

		std::tuple<std::filesystem::path, std::filesystem::path>
		getPairPath(PackageTrioType trioType)
		{
			std::filesystem::path tocPath = m_directory;
			std::filesystem::path cachePath = m_directory;
		
			std::string trioChar = "";
			switch (trioType)
			{
			case PackageTrioType::H:
				trioChar = "H.";
				break;
			case PackageTrioType::F:
				trioChar = "F.";
				break;
			case PackageTrioType::B:
				trioChar = "B.";
				break;
			}
		
			tocPath /= trioChar + m_name + ".toc";
			cachePath /= trioChar + m_name + ".cache";
		
			return { tocPath, cachePath };
		}
	};
	 
	////////////////////////////////////////////////////
	// Specializations
	
	template<>
	inline
	void
	Package<CachePairMeta>::_loadPkgPairs()
	{
		for (int i = 0; i < 3; i++)
		{
			auto pair = getPairPath((PackageTrioType)i);
			if (std::filesystem::exists(std::get<0>(pair)))
			{
				std::shared_ptr<LotusLib::CachePairMeta> x = std::make_shared<LotusLib::CachePairMeta>(std::get<0>(pair), std::get<1>(pair), m_isPostEnsmallening);
				m_pkgs[i] = x;
			}
			else
			{
				m_log.debug(spdlog::fmt_lib::format("Package does not exist: {}", std::get<0>(pair).stem().string()));
			}
		}
	}
}
