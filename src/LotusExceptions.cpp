#include "LotusExceptions.h"

using namespace LotusLib;

LotusException::LotusException(const std::string& msg)
    : runtime_error(msg)
{
}

InvalidDataException::InvalidDataException(const std::string& msg)
    : LotusException(msg)
{}

DecompressionException::DecompressionException(const std::string& msg)
    : LotusException(msg)
{}

InvalidLotusPath::InvalidLotusPath(const std::string& msg)
    : LotusException(msg)
{}