#include "LotusExceptions.h"

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