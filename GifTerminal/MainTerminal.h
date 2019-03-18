#pragma once
#include "Terminal.h"
#include "ConsoleColor.h"

class MainTerminal
{
public:
	MainTerminal();
	~MainTerminal() = default;

	void Lounch();
private:
	ns_terminal::Terminal terminal;
};

