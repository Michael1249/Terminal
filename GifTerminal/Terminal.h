#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include <fstream>


/*Some macroses to use inside Terminal command*/

// use this to declarate function, which can execute by terminal or use as function pointer type
#define TERMINAL_COMMAND(name) std::string name(ns_terminal::Terminal* terminal, ns_terminal::command_args_t& args)

// use this inside TERMINAL_COMMAND body only
#define OUT_STREAM (*terminal->GetOutputStream())
#define IN_STREAM (*terminal->GetInputStream())
#define COMMAND_OUT OUT_STREAM << args[0] << ">> "
#define COMMAND_IN OUT_STREAM << args[0] << "<< "; IN_STREAM 

#define CHECK_ARGUMENTS_NUMBER_EQUAL(expected)	\
if (args.size() != expected + 1)	\
{	\
	std::string str;	\
	str += std::to_string(args.size() - 1);	\
	str += " arguments were found, but the expected ";	\
	str += std::to_string(expected);	\
	throw std::exception(str.c_str());	\
}
#define CHECK_ARGUMENTS_NUMBER_BIGGER_THEN(expected)	\
if (args.size() <= expected + 1)	\
{	\
	std::string str;	\
	str += std::to_string(args.size() - 1);	\
	str += " arguments were found, but the expected ";	\
	str += std::to_string(expected + 1);	\
	str += " or more";	\
	throw std::exception(str.c_str());	\
}

//namespace
namespace ns_terminal
{
	//Terminal Buffer (work like programm stack, but can read extern command)
	class TerminalBuf : public std::streambuf
	{
	public:
		TerminalBuf() = delete;
		TerminalBuf(const TerminalBuf&) = delete;
		TerminalBuf(std::istream*, std::ostream*);

		void AddMacros(std::string&);

		int uflow() override;
		int underflow() override;
	private:
		std::istream* in;
		std::ostream* out;
		std::stack<char> macroses_memory;
	};

	//types which used in terminal
	class Terminal;
	class ICommandDelegate;

	using command_args_t = std::vector<std::string>;
	using command_t = TERMINAL_COMMAND((*));
	using command_list_t = std::map < std::string, ICommandDelegate* >;
	using variable_container_t = std::multimap<std::string, std::string>;

	//base for all terminall comands (executable)
	class ICommandDelegate
	{
	public:
		virtual ~ICommandDelegate() {};
		virtual TERMINAL_COMMAND(operator()) = 0;
	};

	class SimpleCommandDelegate: public ICommandDelegate
	{
	public:
		SimpleCommandDelegate(command_t func);
		TERMINAL_COMMAND(operator());
	private:
		command_t command;
	};

	class MacroCommandDelegate : public ICommandDelegate
	{
	public:
		MacroCommandDelegate(std::string body);
		TERMINAL_COMMAND(operator());
	private:
		std::string command_body;
	};

	template<class T>
	class MemberCommandDelegate: public ICommandDelegate
	{
		using m_command_t = TERMINAL_COMMAND((T::*));
	public:
		MemberCommandDelegate() = delete;
		MemberCommandDelegate(T* obj, m_command_t  m_func):
			m_command(m_func),
			target(obj)
		{
		}
		TERMINAL_COMMAND(operator())
		{
		   return (target->*m_command)(terminal, args);
		}
	private:
		m_command_t m_command;
		T* target;
	};

	class TerminalVariable
	{
	public:
		TerminalVariable(std::string& var_name,variable_container_t& memory);
		~TerminalVariable();

		const std::string& Value() const;
		std::string& Value();

		bool Exist();
	private:
		std::string* val;
		bool exist;
	};

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
		friend TERMINAL_COMMAND(AddVar);
		friend TERMINAL_COMMAND(ShowMemory);

	public:
		Terminal() = delete;
		Terminal(const Terminal&) = delete;
		virtual ~Terminal();

		Terminal(std::string name, std::istream& input, std::ostream& output);
		Terminal(std::string name, std::istream& input, std::ostream& output, command_list_t init_list);

		void AddCommand(std::string name, command_t command);
		void AddMacroCommand(std::string name, std::string macros_body);

		template<class T, typename T_command = TERMINAL_COMMAND((T::*))>
		void AddCommand(std::string name, T* obj, T_command command)
		{
			command_list[name] = new MemberCommandDelegate<T>(obj, command);
		}

		void AskForCommand();

		void Lounch();
		void Finish();

		static void ArgsToString(size_t begin, command_args_t& args, std::string& str);

		TerminalVariable GetVariable(std::string& var_name);
		bool RemoveVariable(std::string& var_name);

		std::istream* GetInputStream() const;
		std::ostream* GetOutputStream() const;
		const std::string& GetName() const;
		const command_list_t& GetCommandList() const;
		TerminalBuf& GetBuffer();
		bool IsLounched() const;
	protected:
		virtual void AddInternCommand();
	private:
		std::string terminal_name;
		TerminalBuf buffer;
		std::istream input_stream;
		std::ostream* output_stream;
		command_list_t command_list;
		variable_container_t memory;

		bool is_lounched;

		void UnknownCommand(std::string);
	};


}