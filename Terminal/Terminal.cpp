#include "pch.h"
#include "Terminal.h"

using namespace ns_terminal;

ns_terminal::Terminal::~Terminal()
{
	for (auto iter = command_list.begin(); iter != command_list.end(); iter++)
	{
		 delete (*iter).second;
	}
}

Terminal::Terminal(std::string name, std::istream & input, std::ostream & output):
	terminal_name(name),
	buffer(&input, &output),
	input_stream(&buffer),
	output_stream(&output)
{
	AddInternCommand();
}

Terminal::Terminal(std::string name, std::istream & input, std::ostream & output, const command_list_t& init_list):
	terminal_name(name),
	buffer(&input, &output),
	input_stream(&buffer),
	output_stream(&output)
{
	for (auto iter = init_list.begin(); iter != init_list.end(); iter++)
	{
		command_list[(*iter).first] = (*iter).second;
	}
	AddInternCommand();
}

void Terminal::AddCommand(std::string name, command_t command)
{
	command_list[name] = new SimpleCommandDelegate(command);
}

void ns_terminal::Terminal::AddMacroCommand(std::string name, std::string macros_body)
{
	command_list['#' + name] = new MacroCommandDelegate(macros_body);
}

void Terminal::AskForCommand()
{
	*output_stream << terminal_name <<"<< ";

	command_args_t args;

	std::string temp;
	do
	{
		input_stream >> temp;
		args.push_back(temp);
	} while (input_stream.peek() != '\n');
	
	try
	{
		auto command_iter = command_list.find(args[0]);
		if (command_iter != command_list.end())
		{
			std::string str = (*(*command_iter).second)(this, args);
			if (str.size() > 0)
			{
				*output_stream << terminal_name << ">> " << args[0] << ": \"" << str << "\"\n" ;
			}
		}
		else
		{
			TerminalVariable curr_var = GetVariable(args[0]);
			if (curr_var.Exist())
			{
				if (args.size() == 1)
				{
					*output_stream << terminal_name << ">> " << args[0] << ": \"" << curr_var.Value() << "\"\n";
					return;
				}
				if (args.size() >= 3)
				{

					args.erase(args.begin());
					std::string operation = args[0];
					if (operation == "<=")
					{
						args.erase(args.begin());
						auto command_iter = command_list.find(args[0]);

						if (command_iter != command_list.end())
						{
							curr_var.Value() = (*(*command_iter).second)(this, args);
						}
						else 
						{
							TerminalVariable other_var = GetVariable(args[0]);
							if (other_var.Exist())
							{
								curr_var.Value() = other_var.Value();
							}
							else
							{
								UnknownCommand(args[0]);
							}
						}
					}
					else if(operation == "=")
					{
						Terminal::ArgsToString(1, args, curr_var.Value());
					}
					else
					{
						UnknownCommand(operation);
					}
				}
				else
				{
					throw std::exception("Wrong number of arguments");
				}
			}
			else
			{
				UnknownCommand(args[0]);
			}
		}
	}
	catch (const std::exception& exc)
	{
		*output_stream << terminal_name << ">> /FATAL COMMAND ERROR: " << exc.what() << '\n';
	}

}

void ns_terminal::Terminal::Import(std::string path)
{
	buffer.AddMacros(FileToString(path));
}

void ns_terminal::Terminal::Launch()
{
	*output_stream << terminal_name <<">> /Terminal started\n";
	is_lounched = true;
	do
	{
		AskForCommand();
	} while (is_lounched);
}

void ns_terminal::Terminal::Finish()
{
	is_lounched = false;
}

std::istream * ns_terminal::Terminal::GetInputStream() const
{
	return (std::istream *)&input_stream;
}

std::ostream * ns_terminal::Terminal::GetOutputStream() const
{
	return output_stream;
}

const std::string & ns_terminal::Terminal::GetName() const
{
	return terminal_name;
}

const command_list_t& ns_terminal::Terminal::GetCommandList() const
{
	return command_list;
}

TerminalBuf& ns_terminal::Terminal::GetBuffer()
{
	return buffer;
}

variable_container_t & ns_terminal::Terminal::GetMemory()
{
	return memory;
}

bool ns_terminal::Terminal::IsLounched() const
{
	return is_lounched;
}

void ns_terminal::Terminal::AddInternCommand()
{
	AddCommand("//", Comment);
	AddCommand("/help", Help);
	AddCommand("/exit", Exit);
	AddCommand("/macros", AddMacros);
	AddCommand("/fmacros", AddFileMacros);
	AddCommand("/var", AddVar);
	AddCommand("/~var", RemoveVar);
	AddCommand("/vars", ShowMemory);
	AddCommand("/fread", ReadFile);
}

void ns_terminal::Terminal::UnknownCommand(std::string s)
{
	*output_stream << terminal_name << ">> /\"" + s + "\" is unknown command!\n";
}

void ns_terminal::Terminal::ArgsToString(size_t begin, command_args_t& args, std::string& str)
{
	str = args[begin];
	for (size_t iter = begin + 1; iter < args.size(); iter++)
	{
		str += ' ' + args[iter];
	}
}

std::string ns_terminal::Terminal::FileToString(std::string& path)
{
	std::string val;
	std::ifstream file;
	file.open(path);
	char c;
	while (file.get(c))
	{
		val += c;
	}
	file.close();
	return val;
}

TerminalVariable ns_terminal::Terminal::GetVariable(std::string & var_name)
{
	return TerminalVariable(var_name, memory);
}


TERMINAL_COMMAND(ns_terminal::Comment)
{
	return "";
}

TERMINAL_COMMAND(ns_terminal::Help)
{
	for (auto iter = terminal->GetCommandList().begin(); iter != terminal->GetCommandList().end(); iter++)
	{
		OUT_STREAM << "   " << ((*iter).first) << '\n';
	}
	return "";
}

TERMINAL_COMMAND(ns_terminal::Exit)
{
	CHECK_ARGUMENTS_NUMBER_EQUAL(0);
	OUT_STREAM << terminal->GetName() << ">> /Terminal finished\n";
	terminal->Finish();
	return "";
}

TERMINAL_COMMAND(ns_terminal::AddMacros)
{
	CHECK_ARGUMENTS_NUMBER_EQUAL(1);
	
	std::string macros_body;
	std::string temp;
	IN_STREAM.get();
	for(;;)
	{
		getline(IN_STREAM, temp);
		if (temp == "")
			break;
		macros_body += temp + '\n';
	} 
	terminal->AddMacroCommand(args[1], macros_body);
	return "";
}

TERMINAL_COMMAND(ns_terminal::AddFileMacros)
{
	CHECK_ARGUMENTS_NUMBER_EQUAL(2);

	terminal->AddMacroCommand(args[0], ns_terminal::Terminal::FileToString(args[2]));
	return "";
}

TERMINAL_COMMAND(ns_terminal::AddVar)
{
	CHECK_ARGUMENTS_NUMBER_BIGGER_THEN(0);
	std::string val = "";
	if (args.size() > 2)
	{
		Terminal::ArgsToString(2, args, val);
	}
	terminal->GetMemory().insert(std::pair<std::string, std::string>(args[1], val));
	return "";
}

TERMINAL_COMMAND(ns_terminal::RemoveVar)
{
	CHECK_ARGUMENTS_NUMBER_EQUAL(1);
	if (const_cast<Terminal*>(terminal)->RemoveVariable(args[1]))
	{
		return "";
	}
	std::string exc_text = "can't remove variable \"" + args[1] + "\"\n";
	throw std::exception(exc_text.c_str());
}


TERMINAL_COMMAND(ns_terminal::ShowMemory)
{
	for (auto iter = terminal->GetMemory().begin(); iter != terminal->GetMemory().end(); iter++)
	{
		OUT_STREAM << "   " << ((*iter).first) << ": \"" << ((*iter).second) << "\"\n";
	}
	return "";
}

TERMINAL_COMMAND(ns_terminal::ReadFile)
{
	CHECK_ARGUMENTS_NUMBER_EQUAL(1);
	TerminalVariable path = terminal->GetVariable(args[1]);
	return ns_terminal::Terminal::FileToString(path.Value());
}

bool ns_terminal::Terminal::RemoveVariable(std::string & var_name)
{
	if (memory.size())
	{
		auto last = --memory.equal_range(var_name).second;
		if (last != memory.end())
		{
			if ((*last).first == var_name)
			{
				memory.erase(last);
				return true;
			}
		}
	}
	return false;
}

