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

// TODO: Decode output redirection here and open files and shit
/*
vector<instruction_t> decode(string input) {
  vector<instruction_t> instrs;
  bool is_foreground = true;
  vector<command_t> cmds;
  command_t *cmd;
  vector<string> instruction_strings, command_strings;
  instruction_strings = separate(input, SYMBOL_INSTRUCTION_DELIMITER);
  for (vector<string>::iterator instr_it = instruction_strings.begin(); instr_it != instruction_strings.end(); instr_it++) {
    command_strings = separate(*instr_it, SYMBOL_PIPE);
    for (vector<string>::iterator cmd_it = command_strings.begin(); cmd_it != command_strings.end(); cmd_it++) {
      cmd = new command_t(separate(*cmd_it, SYMBOL_ARG_DELIMITER));
      if (cmd->args.back() == "&") {
	cmd->args.pop_back();
	is_foreground = false;
      }
      cmds.push_back(*cmd);
    }
    instrs.push_back(instruction_t(cmds, is_foreground));
    is_foreground = true;
  }
  return instrs;
}*/

instruction_t decode_instruction(string input) {
  bool is_foreground = true;
  write_mode_t write_mode = WRITE_MODE_NONE;
  string output_file;
  vector<command_t> cmds;
  vector<string> args;
  vector<string> command_strings = separate(input, SYMBOL_PIPE);
  for (string cmd_str : command_strings) {
    args = separate(cmd_str, SYMBOL_ARG_DELIMITER);
    if (args.back() == SYMBOL_BACKGROUND) {
      args.pop_back();
      is_foreground = false;
    }
    if (args.size() >= 3) {
      string write_symbol = args.at(args.size() - 2);
      if (write_symbol == SYMBOL_OUT_REDIRECT_TRUNCATE) {
	write_mode = WRITE_MODE_TRUNCATE;
	output_file = args.back();
	args.pop_back();
	args.pop_back();
      } else if (write_symbol == SYMBOL_OUT_REDIRECT_APPEND) {
	write_mode = WRITE_MODE_APPEND;
	output_file = args.back();
	args.pop_back();
	args.pop_back();
      } else {
	output_file = "";
      }
    }
    cmds.push_back(command_t(args));
  }
  return instruction_t(cmds, is_foreground, output_file, write_mode);
}

vector<instruction_t> decode(string input) {
  vector<instruction_t> instructions;
  vector<string> instruction_strings = separate(input, SYMBOL_INSTRUCTION_DELIMITER);
  for (string str : instruction_strings) {
    instructions.push_back(decode_instruction(str));
  }
  return instructions;
}
