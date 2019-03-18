#pragma once
#include <iostream>
#include <Windows.h>
namespace console_color
{
	using std::ostream;
	enum TXT {
		TXT_BLACK,
		TXT_BLUE,
		TXT_GREEN,
		TXT_CYAN,
		TXT_RED,
		TXT_MAGENTA,
		TXT_BROWN,
		TXT_LIGHTGRAY,
		TXT_DARKGRAY,
		TXT_LIGHTBLUE,
		TXT_LIGHTGREEN,
		TXT_LIGHTCYAN,
		TXT_LIGHTRED,
		TXT_LIGHTMAGENTA,
		TXT_YELLOW,
		TXT_WHITE
	};
	enum BG {
		BG_BLACK,
		BG_BLUE = 1 << 4,
		BG_GREEN,
		BG_CYAN,
		BG_RED,
		BG_MAGENTA,
		BG_BROWN,
		BG_LIGHTGRAY,
		BG_DARKGRAY,
		BG_LIGHTBLUE,
		BG_LIGHTGREEN,
		BG_LIGHTCYAN,
		BG_LIGHTRED,
		BG_LIGHTMAGENTA,
		BG_YELLOW,
		BG_WHITE
	};
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	const WORD TXT_BIT_MASK = 0xF;
	const WORD BG_BIT_MASK = 0xF0;

	static WORD current_color = BG_BLACK | TXT_LIGHTGRAY;
	ostream& operator<<(ostream&, TXT);
	ostream& operator<<(ostream&, BG);
	void SetConsoleColor(WORD);

	class ConsoleColorGuard
	{
	public:
		ConsoleColorGuard();
		~ConsoleColorGuard();
	private:
		WORD color;
	};

}