#include "LotusLib/EENotationParser.h"

using namespace LotusLib;

nlohmann::json
EENotationParser::parse(const char* data, size_t dataSize)
{
    std::stack<nlohmann::json*> stack;
    std::string key;
    std::string buf;

    auto root = nlohmann::json::object();
    stack.push(&root);
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
                dischargeBuffer(stack, buf, key);
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
                    pushAndIndentObject(stack, key);
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
                    pushAndIndentArray(stack, key);
                }
                pending_obj_or_arr = true;
                break;
            }

            case(','):
            {
                if (pending_obj_or_arr)
                {
                    pending_obj_or_arr = false;
                    pushAndIndentArray(stack, key);
                    dischargeBuffer(stack, buf, key);
                }
                else
                {
                    if (!buf.empty())
                    {
                        dischargeBuffer(stack, buf, key);
                    }
                }
                break;
            }

            case('}'):
            {
                if (pending_obj_or_arr)
                {
                    pending_obj_or_arr = false;
                    pushAndIndentArray(stack, key);
                }
                if (!buf.empty())
                {
                    dischargeBuffer(stack, buf, key);
                }
                stack.pop();
                break;
            }

            case('"'):
            {
                if (pending_obj_or_arr)
                {
                    pending_obj_or_arr = false;
                    pushAndIndentArray(stack, key);
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
                    dischargeBuffer(stack, buf, key);
                }
                break;
            }
            
            default:
            {
                buf.push_back(curChar);
            }
        }
    }
    while (stack.size() != 0)
        stack.pop();
    return root;
}

void
EENotationParser::dischargeBuffer(std::stack<nlohmann::json*>& stack, std::string& buf, std::string& key)
{
    char* endptr;
    auto ival = strtoll(buf.data(), &endptr, 10);
    if (endptr == buf.data() + buf.size())
    {
        if (buf.empty())
        {
            std::string p(buf.data(), buf.size());
            pushValue(stack, key, p);
        }
        else
        {
            pushValue(stack, key, ival);
        }
    }
    else if (endptr != buf.data())
    {
        auto fval = strtod(buf.data(), &endptr);
        if (endptr == buf.data() + buf.size())
        {
            pushValue(stack, key, fval);
        }
        else
        {
            std::string p(buf.data(), buf.size());
            pushValue(stack, key, p);
        }
    }
    else
    {
        std::string p(buf.data(), buf.size());
        pushValue(stack, key, p);
    }
    buf.clear();
}

void
EENotationParser::pushAndIndentArray(std::stack<nlohmann::json*>& stack, std::string& key)
{
    auto value = nlohmann::json::array();
    auto inserted = pushAndGetValue(stack, key, value);
    stack.push(inserted);
}

void
EENotationParser::pushAndIndentObject(std::stack<nlohmann::json*>& stack, std::string& key)
{
    auto value = nlohmann::json::object();
    auto inserted = pushAndGetValue(stack, key, value);
    stack.push(inserted);
}
