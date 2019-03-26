#pragma once
#include "Terminal.h"
#include "TerminalHeader.h"

//Some macroses to use inside Terminal command

// use this inside TERMINAL_COMMAND body only
#define OUT_STREAM (*terminal->getOutputStream())
#define IN_STREAM (*terminal->getInputStream())
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
	//base for all terminall comands (executable)
	class ICommandDelegate
	{
	public:
		virtual ~ICommandDelegate() {};
		virtual TERMINAL_COMMAND(operator()) = 0;
	};

	//simple delegate for none member command
	class SimpleCommandDelegate : public ICommandDelegate
	{
	public:
		SimpleCommandDelegate(command_ptr_t func);
		TERMINAL_COMMAND(operator());
	private:
		command_ptr_t command;
	};

	//macrocommand which insert self body (string) in terminal buffer
	class MacroCommandDelegate : public ICommandDelegate
	{
	public:
		MacroCommandDelegate(const std::string& body);
		TERMINAL_COMMAND(operator());
	private:
		std::string command_body;
	};

	//delegate for member command
	template<class T>
	class MemberCommandDelegate : public ICommandDelegate
	{
		using m_command_ptr_t = TERMINAL_COMMAND((T::*));
	public:
		MemberCommandDelegate() = delete;

		MemberCommandDelegate(T* obj, m_command_ptr_t  m_func) :
			m_command(m_func),
			target(obj)
		{
		}

		TERMINAL_COMMAND(operator())
		{
			return (target->*m_command)(terminal, args);
		}

	private:
		m_command_ptr_t m_command;
		T* target;
	};
}