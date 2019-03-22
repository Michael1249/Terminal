#pragma once
#include <string>
#include "TerminalHeader.h"

namespace ns_terminal
{
	class TerminalVariable
	{
	public:
		TerminalVariable(std::string& var_name, variable_container_t& memory);
		~TerminalVariable();

		const std::string& Value() const;
		std::string& Value();

		bool Exist();
	private:
		std::string* val;
		bool exist;
	};
}
