#pragma once

#include <exception>
#include <stdexcept>
#include <sstream>
#include <string>

namespace LotusLib
{
    class LotusException : public std::runtime_error {
    public:
        LotusException(std::string msg);
        virtual const char* what() const throw();

    private:
        std::string _msg;
    };

    class InvalidDataException : public LotusException {
    public:

        InvalidDataException(std::string msg);
        virtual const char* what() const throw();

    private:
        std::string _msg;
    };

    class DecompressionException : public LotusException {
    public:

        DecompressionException(std::string msg);
        virtual const char* what() const throw();

    private:
        std::string _msg;
    };

    class InvalidLotusPath : public LotusException {
    public:

        InvalidLotusPath(std::string msg);
        virtual const char* what() const throw();

    private:
        std::string _msg;
    };
}
