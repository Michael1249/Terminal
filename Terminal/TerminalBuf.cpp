#include "TerminalBuf.h"


ns_terminal::TerminalBuf::TerminalBuf(std::istream* input, std::ostream* output) :
	in(input),
	out(output)
{
}

void ns_terminal::TerminalBuf::addMacros(const std::string& body)
{
	for (auto iter = body.rbegin(); iter != body.rend(); iter++)
	{
		macroses_memory.push(*iter);
	}
}

int ns_terminal::TerminalBuf::uflow()
{
	if (macroses_memory.empty())
		return in->get();
	char c = macroses_memory.top();
	macroses_memory.pop();
	if (!macroses_memory.empty())
	{
		if (macroses_memory.top() == '\n')
		{
			out->put(c);
			out->put('\n');
		}
		else if (c != '\n')
		{
			out->put(c);
		}
	}
	return c;
}

int ns_terminal::TerminalBuf::underflow()
{
	if (macroses_memory.empty())
		return in->peek();
	return macroses_memory.top();
}
