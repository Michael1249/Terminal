#include "ConsoleColor.h"
using namespace console_color;

ostream & console_color::operator<<(ostream &out, TXT color)
{
	current_color = current_color & BG_BIT_MASK | color;
	SetConsoleTextAttribute(hConsole, current_color);
	return out;
}

ostream & console_color::operator<<(ostream &out, BG color)
{
	current_color = current_color & TXT_BIT_MASK | color;
	SetConsoleTextAttribute(hConsole, current_color);
	return out;
}

void console_color::setConsoleColor(WORD color)
{
	current_color = color;
	SetConsoleTextAttribute(hConsole, current_color);
}
console_color::ConsoleColorGuard::ConsoleColorGuard():
	color(current_color)
{
}

console_color::ConsoleColorGuard::~ConsoleColorGuard()
{
	reset();
}

void console_color::ConsoleColorGuard::reset()
{
	current_color = color;
	SetConsoleTextAttribute(hConsole, color);
}
