#include "LotusExceptions.h"

using namespace LotusLib;

LotusException::LotusException(std::string msg)
    : runtime_error(msg)
{
}

const char* LotusException::what() const throw()
{
    return _msg.c_str();
}

InvalidDataException::InvalidDataException(std::string msg)
    : LotusException(msg)
{}

const char* InvalidDataException::what() const throw()
{
    return _msg.c_str();
}

DecompressionException::DecompressionException(std::string msg)
    : LotusException(msg)
{}

const char* DecompressionException::what() const throw()
{
    return _msg.c_str();
}

InvalidLotusPath::InvalidLotusPath(std::string msg)
    : LotusException(msg)
{}

const char* InvalidLotusPath::what() const throw()
{
    return _msg.c_str();
}