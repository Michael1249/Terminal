#include "pch.h"
#include "CommandDelegate.h"

ns_terminal::MacroCommandDelegate::MacroCommandDelegate(std::string body) :
	command_body(body)
{
}

ns_terminal::SimpleCommandDelegate::SimpleCommandDelegate(command_t func) :
	command(func)
{
}

TERMINAL_COMMAND(ns_terminal::MacroCommandDelegate::operator())
{
	terminal->GetBuffer().AddMacros(this->command_body);
	return "";
}

TERMINAL_COMMAND(ns_terminal::SimpleCommandDelegate::operator())
{
	return command(terminal, args);
}