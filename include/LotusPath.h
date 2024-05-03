#pragma once

#include "LotusExceptions.h"

#include <filesystem>

namespace LotusLib
{
	// Statically constraining this is unecessary
	// Just make this a forward-slash path
	// ex. /Lotus/Characters/Tenno/Excalibur/ExcaliburBody_skel.fbx
	using LotusPath = std::filesystem::path;
}
