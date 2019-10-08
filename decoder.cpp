#include"decoder.h"

#include<vector>
#include<string>

using namespace std;

#define SYMBOL_ARG_DELIMITER                    " "
#define SYMBOL_INSTRUCTION_DELIMITER            ";"
#define SYMBOL_PIPE                             "|"
#define SYMBOL_BACKGROUND                       "&"
#define SYMBOL_OUT_REDIRECT_TRUNCATE            ">"
#define SYMBOL_OUT_REDIRECT_APPEND              ">>"

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

vector<instruction_t> decode(string input) {
  vector<instruction_t> instructions;
  vector<string> instruction_strings = separate(input, SYMBOL_INSTRUCTION_DELIMITER);
  for (string str : instruction_strings) {
    instructions.push_back(decode_instruction(str));
  }
  return instructions;
}
