#include "TerminalVariable.h"

ns_terminal::TerminalVariable::TerminalVariable(std::string& var_name, variable_container_t & memory)
{
	if (memory.size())
	{
		auto last = --memory.equal_range(var_name).second;
		if (last != memory.end())
		{
			if ((*last).first == var_name)
			{
				val = &(*last).second;
				exist = true;
				return;
			}
		}
	}
	exist = false;
	val = new std::string(var_name);
}

ns_terminal::TerminalVariable::~TerminalVariable()
{
	if (!exist)
	{
		delete val;
	}
}

const std::string & ns_terminal::TerminalVariable::Value() const
{
	return *val;
}

std::string & ns_terminal::TerminalVariable::Value()
{
	return *val;
}


bool ns_terminal::TerminalVariable::isExist()
{
	return exist;
}