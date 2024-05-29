#include "LotusNotationParser.h"

using namespace LotusLib;

nlohmann::json
LotusNotationParser::parse(char* data, size_t dataSize)
{
    auto root = nlohmann::json::object();
    m_stack.push(&root);
    bool pending_obj_or_arr = false;
    bool is_string = false;
    for (size_t i = 0; i < dataSize; i++)
    {
        const char& curChar = data[i];

        if (is_string)
        {
            if (curChar == '"')
            {
                is_string = false;
                dischargeBuffer();
            }
            else
            {
                buf.push_back(curChar);
            }
            continue;
        }

        switch(curChar)
        {
            case(' '):
            case('\0'):
            case('\r'):
                continue;

            case('='):
            {
                if (pending_obj_or_arr)
                {
                    pending_obj_or_arr = false;
                    pushAndIndentObject();
                    key = std::move(buf);
                }
                else
                {
                    key = std::move(buf);
                }
                buf.clear();
                break;
            }

            case('{'):
            {
                if (pending_obj_or_arr)
                {
                    pushAndIndentArray();
                }
                pending_obj_or_arr = true;
                break;
            }

            case(','):
            {
                if (pending_obj_or_arr)
                {
                    pending_obj_or_arr = false;
                    pushAndIndentArray();
                    dischargeBuffer();
                }
                else
                {
                    if (!buf.empty())
                    {
                        dischargeBuffer();
                    }
                }
                break;
            }

            case('}'):
            {
                if (pending_obj_or_arr)
                {
                    pending_obj_or_arr = false;
                    pushAndIndentArray();
                }
                if (!buf.empty())
                {
                    dischargeBuffer();
                }
                m_stack.pop();
                break;
            }

            case('"'):
            {
                if (pending_obj_or_arr)
                {
                    pending_obj_or_arr = false;
                    pushAndIndentArray();
                }
                if (buf.empty())
                {
                    is_string = true;
                }
                else
                {
                    buf.push_back(curChar);
                }
                break;
            }

            case('\n'):
            {
                if (!buf.empty() && !pending_obj_or_arr)
                {
                    dischargeBuffer();
                }
                break;
            }
            
            default:
            {
                buf.push_back(curChar);
            }
        }
    }
    while (m_stack.size() != 0)
        m_stack.pop();
    return root;
}

void
LotusNotationParser::dischargeBuffer()
{
    char* endptr;
    auto ival = strtoll(buf.data(), &endptr, 10);
    if (endptr == buf.data() + buf.size())
    {
        if (buf.empty())
        {
            std::string p(buf.data(), buf.size());
            pushValue(p);
        }
        else
        {
            pushValue(ival);
        }
    }
    else if (endptr != buf.data())
    {
        auto fval = strtod(buf.data(), &endptr);
        if (endptr == buf.data() + buf.size())
        {
            pushValue(fval);
        }
        else
        {
            std::string p(buf.data(), buf.size());
            pushValue(p);
        }
    }
    else
    {
        std::string p(buf.data(), buf.size());
        pushValue(p);
    }
    buf.clear();
}

void
LotusNotationParser::pushAndIndentArray()
{
    auto value = nlohmann::json::array();
    auto inserted = pushAndGetValue(value);
    m_stack.push(inserted);
}

void
LotusNotationParser::pushAndIndentObject()
{
    auto value = nlohmann::json::object();
    auto inserted = pushAndGetValue(value);
    m_stack.push(inserted);
}
