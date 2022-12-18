#include "LotusExceptions.h"

using namespace LotusLib;

LotusException::LotusException(const std::string& msg)
    : runtime_error(msg)
{
}

std::ostringstream LotusException::cnvt;
const char* LotusException::what() const throw()
{
    cnvt.str("");
    cnvt << runtime_error::what();
    return cnvt.str().c_str();
}

InvalidDataException::InvalidDataException(const std::string& msg)
    : LotusException(msg)
{}

std::ostringstream InvalidDataException::cnvt;
const char* InvalidDataException::what() const throw()
{
    cnvt.str("");
    cnvt << runtime_error::what();
    return cnvt.str().c_str();
}

DecompressionException::DecompressionException(const std::string& msg)
    : LotusException(msg)
{}

std::ostringstream DecompressionException::cnvt;
const char* DecompressionException::what() const throw()
{
    cnvt.str("");
    cnvt << runtime_error::what();
    return cnvt.str().c_str();
}

InvalidLotusPath::InvalidLotusPath(const std::string& msg)
    : LotusException(msg)
{}

std::ostringstream InvalidLotusPath::cnvt;
const char* InvalidLotusPath::what() const throw()
{
    cnvt.str("");
    cnvt << runtime_error::what();
    return cnvt.str().c_str();
}