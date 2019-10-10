#include"decoder.h"

#include<vector>
#include<string>
#include<cstring>

using namespace std;

#define SYMBOL_ARG_DELIMITER                    " "
#define SYMBOL_INSTRUCTION_DELIMITER            ";"
#define SYMBOL_PIPE                             "|"
#define SYMBOL_BACKGROUND                       "&"
#define SYMBOL_IN_REDIRECT                      "<"
#define SYMBOL_OUT_REDIRECT_TRUNCATE            ">"
#define SYMBOL_OUT_REDIRECT_APPEND              ">>"

void trim(string str) {
  while (str.front() == ' ') {
    str.erase(0);
  }
  while (str.back() == ' ') {
    str.pop_back();
  }
}

vector<string> separate(string str, string delimiter) {
  vector<string> substrings;
  char *c_str = new char[str.length()];
  strcpy(c_str, str.c_str());
  char *str_buf;
  while ((str_buf = strsep(&c_str, delimiter.c_str())) != NULL) {
    if (strlen(str_buf) == 0) {
      continue;
    }
    substrings.push_back(str_buf);
  }
  return substrings;
}

/*string substring_after(string str, string sequence, char stop_at) {
  string str_cpy = str;
  size_t start_index, end_index;
  if ((start_index = str_copy.rfind(sequence)) == npos) {
    return "";
  }
  str_copy = str.substring(start_index + sequence.length());
  if ((end_index = str_copy.rfind(stop_at)) != npos) {
    return str_copy.substring(0, end_index);
  } else {
    return str_copy;
  }
  }*/

instruction_t decode_instruction(string input) {
  instruction_t instruction;
  string output_file;
  vector<command_t> cmds;
  vector<string> args;
  vector<string> command_strings = separate(input, SYMBOL_PIPE);
  for (string cmd_str : command_strings) {
    args = separate(cmd_str, SYMBOL_ARG_DELIMITER);
    if (args.back() == SYMBOL_BACKGROUND) {
      args.pop_back();
      instruction.is_foreground = false;
    }
    if (args.size() >= 3) {
      string write_symbol = args.at(args.size() - 2);
      if (write_symbol == SYMBOL_OUT_REDIRECT_TRUNCATE) {
	instruction.write_mode = WRITE_MODE_TRUNCATE;
	instruction.output_file = args.back();
	args.pop_back();
	args.pop_back();
      } else if (write_symbol == SYMBOL_OUT_REDIRECT_APPEND) {
	instruction.write_mode = WRITE_MODE_APPEND;
	instruction.output_file = args.back();
	args.pop_back();
	args.pop_back();
      }
    }
    cmds.push_back(command_t(args));
  }
  instruction.cmds = cmds;
  return instruction;
}

/*instruction_t decode_instruction(string input) {
  instruction_t instruction;
  vector<string> args = separate(input, SYMBOL_ARG_DELIMITER);

  if (args.back() == SYMBOL_BACKGROUND) {// Find &
    instruction.is_foreground = false;
    args.pop_back();
  }

  auto str_it = args.end();// Find file redirection
  while (1) {
    str_it = args.end() - 1;
    if (*(str_it - 1) == SYMBOL_IN_REDIRECT) {
      instruction.input_file = args.back();
    } else if (*(str_it - 1) == SYMBOL_OUT_REDIRECT_TRUNCATE) {
      instruction.output_file = args.back();
      instruction.write_mode = WRITE_MODE_TRUNCATE;
    } else if (*(str_it - 1) == SYMBOL_OUT_REDIRECT_APPEND) {
      instruction.output_file = args.back();
      instruction.write_mode = WRITE_MODE_APPEND;
    } else {
      break;
    }
    args.pop_back();
    args.pop_back();
  }

  vector<command_t> cmds;// Get all commands
  auto arg_start_it = args.begin();
  for (auto arg_it = args.begin(); arg_it != args.end(); arg_it++) {
    if (*arg_it == SYMBOL_PIPE) {
      cmds.push_back(command_t(vector<string>(arg_start_it, arg_it)));
      arg_start_it = arg_it + 1;
    }
  }
  instruction.cmds = cmds;

  return instruction;
  }*/

vector<instruction_t> decode(string input) {
  vector<instruction_t> instructions;
  vector<string> instruction_strings = separate(input, SYMBOL_INSTRUCTION_DELIMITER);
  for (string str : instruction_strings) {
    instructions.push_back(decode_instruction(str));
  }
  return instructions;
}
