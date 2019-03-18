#include "pch.h"
#include "MainTerminal.h"

TERMINAL_COMMAND(test)
{
	COMMAND_OUT << "inter the number\n";
	std::string a;
	COMMAND_IN >> a;
	COMMAND_OUT << "your number is " <<a << '\n';
	return "";
}

MainTerminal::MainTerminal():
	terminal("MAIN", std::cin, std::cout)
{
	terminal.AddCommand("test", test);
}

void MainTerminal::Lounch()
{
	terminal.Lounch();
}
