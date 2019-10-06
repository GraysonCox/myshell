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

vector<instruction_t> decode(string input) {
  vector<instruction_t> instrs;
  instruction_t instr;
  command_t cmd;
  vector<string> instruction_strings, command_strings;
  instruction_strings = separate(input, SYMBOL_INSTRUCTION_DELIMITER);
  for (vector<string>::iterator instr_it = instruction_strings.begin(); instr_it != instruction_strings.end(); instr_it++) {
    command_strings = separate(*instr_it, SYMBOL_PIPE);
    for (vector<string>::iterator cmd_it = command_strings.begin(); cmd_it != command_strings.end(); cmd_it++) {
      cmd = separate(*cmd_it, SYMBOL_ARG_DELIMITER);
      //cmd.push_back(NULL);
      instr.push_back(cmd);
    }
    //    instr.push_back(NULL);
    instrs.push_back(instr);
  }
  return instrs;
}
