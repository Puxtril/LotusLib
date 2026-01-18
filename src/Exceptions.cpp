#include "LotusLib/Exceptions.h"

using namespace LotusLib;

LotusException::LotusException(const std::string& msg)
    : runtime_error(msg)
{
}

DecompressionException::DecompressionException(const std::string& msg)
    : LotusException(msg)
{}

CommonHeaderError::CommonHeaderError(const std::string& msg)
    : LotusException(msg)
{}

InternalEntryNotFound::InternalEntryNotFound(const std::string& msg)
    : LotusException(msg)
{}

InternalFileNotFound::InternalFileNotFound(const std::string& msg)
    : InternalEntryNotFound(msg)
{}

InternalDirectoryNotFound::InternalDirectoryNotFound(const std::string& msg)
    : InternalEntryNotFound(msg)
{}

PackageNotFound::PackageNotFound(const std::string& pkg)
    : LotusException("Package doesn't exist: " + pkg)
{}

PackageSplitNotFound::PackageSplitNotFound(const std::string& pkg, char split)
    : LotusException("PackageSplit doesn't exist: " + std::to_string(split) + '.' + pkg)
{}