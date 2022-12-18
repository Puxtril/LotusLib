#pragma once

#include <exception>
#include <stdexcept>
#include <sstream>
#include <string>

namespace LotusLib
{
    class LotusException : public std::runtime_error {
    public:
        LotusException(const std::string& msg);
        virtual const char* what() const throw();

    private:
        static std::ostringstream cnvt;
    };

    class InvalidDataException : public LotusException {
    public:

        InvalidDataException(const std::string& msg);
        virtual const char* what() const throw();

    private:
        static std::ostringstream cnvt;
    };

    class DecompressionException : public LotusException {
    public:

        DecompressionException(const std::string& msg);
        virtual const char* what() const throw();

    private:
        static std::ostringstream cnvt;
    };

    class InvalidLotusPath : public LotusException {
    public:

        InvalidLotusPath(const std::string& msg);
        virtual const char* what() const throw();

    private:
        static std::ostringstream cnvt;
    };
}