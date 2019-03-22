#pragma once
#include <map>
#include <vector>
#include <string>
// use this to declarate function, which can execute by terminal or use as function pointer type
#define TERMINAL_COMMAND(name) std::string name(ns_terminal::Terminal* terminal, ns_terminal::command_args_t& args) 

namespace ns_terminal
{
	//Forward declaration
	class Terminal;
	class ICommandDelegate;
	template<class T>
	class MemberCommandDelegate;

	using command_args_t = std::vector<std::string>;
	using command_t = TERMINAL_COMMAND((*));
	using command_list_t = std::map < std::string, ICommandDelegate* >;
	using variable_container_t = std::multimap<std::string, std::string>;
}