#pragma once
#include <iostream>
#include <fstream>

#include "CommandDelegate.h"
#include "TerminalBuf.h"
#include "TerminalVariable.h"

//namespace
namespace ns_terminal
{
	
	//standart commands
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

		Terminal(std::string name, std::istream& input, std::ostream& output);
		Terminal(std::string name, std::istream& input, std::ostream& output, const command_list_t& init_list);

		//read input (buffer)
		void AskForCommand();

		void Import(std::string path);

		//add SimpleCommandDelegate
		void AddCommand(std::string name, command_t command);

		//add MemberCommandDelegate
		template<class T, typename T_command>
		void AddCommand(std::string name, T* obj, T_command command);

		//add MacroCommandDelegate
		void AddMacroCommand(std::string name, std::string macros_body);

		void Launch();
		void Finish();

		TerminalVariable GetVariable(std::string& var_name);
		bool RemoveVariable(std::string& var_name);

		std::istream* GetInputStream() const;
		std::ostream* GetOutputStream() const;

		const std::string& GetName() const;
		
		const command_list_t& GetCommandList() const;
		
		TerminalBuf& GetBuffer();

		variable_container_t& GetMemory();
		
		bool IsLounched() const;

		//convert arg array back to string
		static void ArgsToString(size_t begin, command_args_t& args, std::string& str);
		static std::string FileToString(std::string& path);

	private:
		std::string terminal_name;

		TerminalBuf buffer;
		
		std::istream input_stream;
		
		std::ostream* output_stream;

		command_list_t command_list;
		
		variable_container_t memory;

		bool is_lounched;

		void AddInternCommand();

		void UnknownCommand(std::string);
	};


}

template<class T, typename T_command>
void ns_terminal::Terminal::AddCommand(std::string name, T* obj, T_command command)
{
	command_list[name] = new MemberCommandDelegate<T>(obj, command);
}