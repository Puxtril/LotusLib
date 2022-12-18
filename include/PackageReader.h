#pragma once

#include "CachePairReader.h"
#include "Package.h"
#include "Logger.h"

#include <filesystem>
#include <string>
#include <optional>
#include <array>

namespace LotusLib
{
	class ConstCachePairReaderIterator : public ConstCachePairIterator
	{
		std::array<std::optional<CachePairReader>, 3>::const_iterator m_iter;

	public:
		using value_type = CachePairReader;
		using size_type = size_t;
		using pointer = const value_type*;
		using reference = const value_type&;

		ConstCachePairReaderIterator();
		ConstCachePairReaderIterator(std::array<std::optional<value_type>, 3> pkgs);

		ConstCachePairReaderIterator& operator++();

		pointer operator*() const;
		pointer operator->() const;

		bool operator==(const ConstCachePairReaderIterator& other) const;
		bool operator!=(const ConstCachePairReaderIterator& other) const;
	};

	class CachePairReaderIterator : public CachePairIterator
	{
		std::array<std::optional<CachePairReader>, 3>::iterator m_iter;

	public:
		using value_type = CachePairReader;
		using size_type = size_t;
		using pointer = value_type*;
		using reference = value_type&;

		CachePairReaderIterator();
		CachePairReaderIterator(std::array<std::optional<value_type>, 3> pkgs);

		CachePairReaderIterator& operator++();

		pointer operator*() const;
		pointer operator->() const;

		bool operator==(const CachePairReaderIterator& other) const;
		bool operator!=(const CachePairReaderIterator& other) const;
	};

	class PackageReader : public Package
	{
		Logger& m_log;
		mutable std::array<std::optional<CachePairReader>, 3> m_pkgs;

	public:
		PackageReader(std::filesystem::path pkgDir, std::string pkgName, bool isPostEnsmallening);

		CachePairReaderIterator* begin();
		CachePairReaderIterator& end();
		ConstCachePairReaderIterator* begin() const;
		ConstCachePairReaderIterator& end() const;

		const CachePairReader* operator[](PackageTrioType trioType) const;

		void loadPkgPairs();

	private:
		void loadPairs();
	};
}
