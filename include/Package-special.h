template <>
void LotusLib::Package<LotusLib::CachePairMeta>::_loadPkgPairs()
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
