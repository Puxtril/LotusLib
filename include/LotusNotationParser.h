#pragma once

#include "nlohmann/json.hpp"

#include <stack>

namespace LotusLib
{
	// Credit to Sainan for this class
	class LotusNotationParser
	{
	protected:
		std::stack<nlohmann::json*> m_stack;
		std::string key;
		std::string buf;

	public:
		[[nodiscard]]
		nlohmann::json
		parse(const char* data, size_t dataSize);

	protected:
		void dischargeBuffer();
		void pushAndIndentArray();
		void pushAndIndentObject();

		template <typename T>
		nlohmann::json*
		pushAndGetValue(T& value)
		{
			if (m_stack.top()->is_array())
			{
				m_stack.top()->push_back(value);
				return &m_stack.top()->back();
			}
			else
			{
				m_stack.top()->operator[](key) = value;
				nlohmann::json* ret = &m_stack.top()->operator[](key);
				key.clear();
				return ret;
			}
		}

		template <typename T>
		void
		pushValue(T& value)
		{
			if (m_stack.top()->is_array())
			{
				m_stack.top()->push_back(value);
			}
			else
			{
				m_stack.top()->operator[](key) = value;
				key.clear();
			}
		}
	};
}
