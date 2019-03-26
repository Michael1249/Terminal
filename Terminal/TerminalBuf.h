#pragma once
#include <fstream>
#include <stack>
namespace ns_terminal
{
	//Terminal Buffer (work like programm stack, but can read extern command)
	class TerminalBuf : public std::streambuf
	{
	public:
		TerminalBuf() = delete;
		TerminalBuf(const TerminalBuf&) = delete;
		TerminalBuf(std::istream*, std::ostream*);

		void addMacros(const std::string&);

		int uflow() override;
		int underflow() override;
	private:
		std::istream* in;
		std::ostream* out;
		std::stack<char> macroses_memory;
	};
}