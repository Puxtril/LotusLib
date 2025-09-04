#pragma once

#include "nlohmann/json.hpp"

#include <stack>

namespace LotusLib
{
	// Credit to Sainan for this class
	// https://github.com/Sainan/ee-notation-parser
	class LotusNotationParser
	{
	public:
		[[nodiscard]]
		static nlohmann::json parse(const char* data, size_t dataSize);

	protected:
		static void dischargeBuffer(std::stack<nlohmann::json*>& stack, std::string& buf, std::string& key);
		static void pushAndIndentArray(std::stack<nlohmann::json*>& stack, std::string& key);
		static void pushAndIndentObject(std::stack<nlohmann::json*>& stack, std::string& key);

		template <typename T>
		static
		nlohmann::json*
		pushAndGetValue(std::stack<nlohmann::json*>& stack, std::string& key, T& value)
		{
			if (stack.top()->is_array())
			{
				stack.top()->push_back(value);
				return &stack.top()->back();
			}
			else
			{
				stack.top()->operator[](key) = value;
				nlohmann::json* ret = &stack.top()->operator[](key);
				key.clear();
				return ret;
			}
		}

		template <typename T>
		static
		void
		pushValue(std::stack<nlohmann::json*>& stack, std::string& key, T& value)
		{
			if (stack.top()->is_array())
			{
				stack.top()->push_back(value);
			}
			else
			{
				stack.top()->operator[](key) = value;
				key.clear();
			}
		}
	};
}
