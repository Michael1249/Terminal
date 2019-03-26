#pragma once
#include <iostream>
#include <fstream>

#include "CommandDelegate.h"
#include "TerminalBuf.h"
#include "TerminalVariable.h"

//namespace
namespace ns_terminal
{
	
	//standard commands
	TERMINAL_COMMAND(Comment);
	TERMINAL_COMMAND(Help);
	TERMINAL_COMMAND(Exit);
	TERMINAL_COMMAND(AddMacros);
	TERMINAL_COMMAND(AddFileMacros);
	TERMINAL_COMMAND(AddVar);
	TERMINAL_COMMAND(ShowMemory);
	TERMINAL_COMMAND(RemoveVar);
	TERMINAL_COMMAND(ReadFile);

	class Terminal
	{

	public:
		Terminal() = delete;
		Terminal(const Terminal&) = delete;
		virtual ~Terminal();

		Terminal(const std::string& name, std::istream& input, std::ostream& output);
		Terminal(const std::string& name, std::istream& input, std::ostream& output, const command_list_t& init_list);

		//read input (buffer)
		void askForCommand();

		void importFile(const std::string& path);

		//add SimpleCommandDelegate
		void addCommand(const std::string& name, command_ptr_t command);

		//add MemberCommandDelegate
		template<class T, typename T_command>
		void addCommand(const std::string& name, T* obj, T_command command);

		//add MacroCommandDelegate
		void addMacroCommand(const std::string& name, std::string macros_body);

		void launch();
		void finish();

		TerminalVariable getVariable(std::string& var_name);
		bool removeVariable(std::string& var_name);

		std::istream* getInputStream() const;
		std::ostream* getOutputStream() const;

		const std::string& getName() const;
		
		const command_list_t& getCommandList() const;
		
		TerminalBuf& getBuffer();

		variable_container_t& getMemory();
		
		bool isLounched() const;

		//convert arg array back to string
		static void argsToString(size_t begin, command_args_t& args, std::string& str);

	private:
		std::string terminal_name;

		TerminalBuf buffer;
		
		std::istream input_stream;
		
		std::ostream* output_stream;

		command_list_t command_list;
		
		variable_container_t memory;

		bool is_lounched;

		void addInternCommand();

		void unknownCommand(std::string);
	};


}

std::string fileToString(const std::string& path);

template<class T, typename T_command>
void ns_terminal::Terminal::addCommand(const std::string& name, T* obj, T_command command)
{
	command_list[name] = new MemberCommandDelegate<T>(obj, command);
}
