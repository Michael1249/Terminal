// GifTerminal.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "MainTerminal.h"

int main(int argc, char* argv[])
{
	std::cout << "|Init\n";
	for (int i = 0; i < argc; i++)
	{
		std::cout << "| " << argv[i] << '\n';
	}
	MainTerminal terminal;
	if (argc == 2)
	{
		terminal.Import(argv[1]);
	}
	terminal.Launch();
}

