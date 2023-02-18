#pragma once

#include "LotusExceptions.h"

#include <filesystem>

namespace LotusLib
{
	//! \brief Wrapper for std::filesystem::path with constraints
	//! 
	//! Constraints:
	//!  * Path must be unix-like (/ delimiter)
	//!  * Path must be absolute
	class LotusPath : public std::filesystem::path
	{
		void checkSeparator()
		{
			if (this->string().find('\\') != std::string::npos)
				throw LotusLib::InvalidLotusPath("LotusPath contains invalid separator: " + this->string());
		}

	public:
		using value_type = char;
		static constexpr value_type preferred_separator = '/';

		LotusPath() = default;		

		LotusPath(const std::filesystem::path& p)
			: std::filesystem::path(p)
		{
			checkSeparator();
		}

		LotusPath(std::filesystem::path&& p) noexcept
			: std::filesystem::path(p)
		{
			checkSeparator();
		}

		LotusPath(string_type&& source, format fmt = auto_format)
			: std::filesystem::path(std::move(source), fmt)
		{
			checkSeparator();
		}

		template< class Source>
		LotusPath(const Source& source, format fmt = auto_format)
			: std::filesystem::path(source, fmt)
		{
			checkSeparator();
		}

		template< class InputIt >
		LotusPath(InputIt first, InputIt last, format fmt = auto_format)
			: std::filesystem::path(first, last, fmt)
		{
			checkSeparator();
		}

		template< class Source >
		LotusPath(const Source& source, const std::locale& loc, format fmt = auto_format)
			: std::filesystem::path(source, loc, fmt)
		{
			checkSeparator();
		}

		template< class InputIt >
		LotusPath(InputIt first, InputIt last, const std::locale& loc, format fmt = auto_format)
			: std::filesystem::path(first, last, loc, fmt)
		{
			checkSeparator();
		}

		std::filesystem::path& make_preferred() noexcept
		{
			return *this;
		}

		std::filesystem::path
		getPreferredPath() const
		{
			std::filesystem::path newPath = *this;
			newPath.make_preferred();
			return newPath;
		}
	};
}
