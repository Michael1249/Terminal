#include "Terminal.h"

using namespace ns_terminal;

ns_terminal::Terminal::~Terminal()
{

	for (auto iter : command_list)
	{
		 delete iter.second;
	}

}

Terminal::Terminal(const std::string& name, std::istream & input, std::ostream & output):
	terminal_name(name),
	buffer(&input, &output),
	input_stream(&buffer),
	output_stream(&output)
{
	addInternCommand();
}

Terminal::Terminal(const std::string& name, std::istream & input, std::ostream & output, const command_list_t& init_list):
	Terminal(name, input, output)
{

	for (auto iter : init_list)
	{
		command_list[iter.first] = iter.second;
	}

}

void Terminal::addCommand(const std::string& name, command_ptr_t command)
{
	command_list[name] = new SimpleCommandDelegate(command);
}

void ns_terminal::Terminal::addMacroCommand(const std::string& name, std::string macros_body)
{
	command_list['#' + name] = new MacroCommandDelegate(macros_body);
}

void Terminal::askForCommand()
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
			TerminalVariable curr_var = getVariable(args[0]);

			if (curr_var.isExist())
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
							TerminalVariable other_var = getVariable(args[0]);

							if (other_var.isExist())
							{
								curr_var.Value() = other_var.Value();
							}
							else
							{
								unknownCommand(args[0]);
							}

						}

					}
					else if(operation == "=")
					{
						Terminal::argsToString(1, args, curr_var.Value());
					}
					else
					{
						unknownCommand(operation);
					}

				}
				else
				{
					throw std::exception("Wrong number of arguments");
				}

			}
			else
			{
				unknownCommand(args[0]);
			}

		}

	}
	catch (const std::exception& exc)
	{
		*output_stream << terminal_name << ">> /FATAL COMMAND ERROR: " << exc.what() << '\n';
	}

}

void ns_terminal::Terminal::importFile(const std::string& path)
{
	buffer.addMacros(fileToString(path));
	size_t index = path.rfind('\\');
	std::string file_name = "Imported" + path.substr(index, path.size() - index);
	command_args_t args {"", file_name, path};
	AddVar(this, args);
}

void ns_terminal::Terminal::launch()
{
	*output_stream << terminal_name <<">> /Terminal started\n";
	is_lounched = true;

	do
	{
		askForCommand();
	} while (is_lounched);

}

void ns_terminal::Terminal::finish()
{
	is_lounched = false;
}

std::istream * ns_terminal::Terminal::getInputStream() const
{
	return (std::istream *)&input_stream;
}

std::ostream * ns_terminal::Terminal::getOutputStream() const
{
	return output_stream;
}

const std::string & ns_terminal::Terminal::getName() const
{
	return terminal_name;
}

const command_list_t& ns_terminal::Terminal::getCommandList() const
{
	return command_list;
}

TerminalBuf& ns_terminal::Terminal::getBuffer()
{
	return buffer;
}

variable_container_t & ns_terminal::Terminal::getMemory()
{
	return memory;
}

bool ns_terminal::Terminal::isLounched() const
{
	return is_lounched;
}

void ns_terminal::Terminal::addInternCommand()
{
	addCommand("//", Comment);
	addCommand("/help", Help);
	addCommand("/exit", Exit);
	addCommand("/macros", AddMacros);
	addCommand("/fmacros", AddFileMacros);
	addCommand("/var", AddVar);
	addCommand("/~var", RemoveVar);
	addCommand("/vars", ShowMemory);
	addCommand("/fread", ReadFile);
}

void ns_terminal::Terminal::unknownCommand(std::string s)
{
	*output_stream << terminal_name << ">> /\"" + s + "\" is unknown command!\n";
}

void ns_terminal::Terminal::argsToString(size_t begin, command_args_t& args, std::string& str)
{
	str = args[begin];

	for (size_t iter = begin + 1; iter < args.size(); iter++)
	{
		str += ' ' + args[iter];
	}

}

std::string fileToString(const std::string& path)
{
	std::string val;
	std::ifstream file;

	file.open(path);
	
	if (!file)
	{
		throw std::exception("Can not open file to read!");
	}

	char c;

	while (file.get(c))
	{
		val += c;
	}

	file.close();
	return val;
}

TerminalVariable ns_terminal::Terminal::getVariable(std::string & var_name)
{
	return TerminalVariable(var_name, memory);
}


TERMINAL_COMMAND(ns_terminal::Comment)
{
	return "";
}

TERMINAL_COMMAND(ns_terminal::Help)
{

	for (auto iter : terminal->getCommandList())
	{
		OUT_STREAM << "   " << iter.first << '\n';
	}

	return "";
}

TERMINAL_COMMAND(ns_terminal::Exit)
{
	CHECK_ARGUMENTS_NUMBER_EQUAL(0);
	OUT_STREAM << terminal->getName() << ">> /Terminal finished\n";
	terminal->finish();
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

		if (temp.empty())
		{
			break;
		}

		macros_body += temp + '\n';
	} 

	terminal->addMacroCommand(args[1], macros_body);
	return "";
}

TERMINAL_COMMAND(ns_terminal::AddFileMacros)
{
	CHECK_ARGUMENTS_NUMBER_EQUAL(2);

	terminal->addMacroCommand(args[1], fileToString(args[2]));
	return "";
}

TERMINAL_COMMAND(ns_terminal::AddVar)
{
	CHECK_ARGUMENTS_NUMBER_BIGGER_THEN(0);
	std::string val = "";
	if (args.size() > 2)
	{
		Terminal::argsToString(2, args, val);
	}
	terminal->getMemory().insert(std::pair<std::string, std::string>(args[1], val));
	return "";
}

TERMINAL_COMMAND(ns_terminal::RemoveVar)
{
	CHECK_ARGUMENTS_NUMBER_EQUAL(1);

	if (terminal->removeVariable(args[1]))
	{
		return "";
	}

	std::string exc_text = "can't remove variable \"" + args[1] + "\"\n";
	throw std::exception(exc_text.c_str());
}


TERMINAL_COMMAND(ns_terminal::ShowMemory)
{

	for (auto iter : terminal->getMemory())
	{
		OUT_STREAM << "   " << iter.first << ": \"" << iter.second << "\"\n";
	}

	return "";
}

TERMINAL_COMMAND(ns_terminal::ReadFile)
{
	CHECK_ARGUMENTS_NUMBER_EQUAL(1);
	TerminalVariable path = terminal->getVariable(args[1]);
	return fileToString(path.Value());
}

bool ns_terminal::Terminal::removeVariable(std::string & var_name)
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

