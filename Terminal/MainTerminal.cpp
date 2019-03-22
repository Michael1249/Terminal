
#include "pch.h"
#include "MainTerminal.h"
using namespace console_color;

TERMINAL_COMMAND(test)
{
	ConsoleColorGuard guard;
	COMMAND_OUT << TXT_CYAN << "enter the number\n";
	std::string a;
	guard.Reset();
	COMMAND_IN >> a;
	COMMAND_OUT << "your number is " <<a << '\n';
	return "";
}


MainTerminal::MainTerminal():
	Terminal("MAIN", std::cin, std::cout)
{
	AddCommand("test", test);
}

