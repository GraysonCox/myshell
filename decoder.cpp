/**

   decoder.cpp

   This module handles the decoding of strings and creation of instruction_t and
   command_t objects. Most of these methods are not specified in decoder.h because
   they are utility/helper functions.

 */

#include"decoder.h"

#include<vector>
#include<string>
#include<cstring>

using namespace std;

#define SYMBOL_ARG_DELIMITER                    " \t"
#define SYMBOL_INSTRUCTION_DELIMITER            ";"
#define SYMBOL_PIPE                             "|"
#define SYMBOL_BACKGROUND                       "&"
#define SYMBOL_IN_REDIRECT                      "<"
#define SYMBOL_OUT_REDIRECT_TRUNCATE            ">"
#define SYMBOL_OUT_REDIRECT_APPEND              ">>"

//
// Separates a string into substrings delimited by the given delimiter. For example,
// separate("a;b;c", ";") would return a vector containing "a", "b", and "c".
//
// @param - str: The string to be separated.
//  - delimiter: The string which delimits the border of the substrings to return.
//
// @return - A vector containing the substrings
//
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

//
// This is called by decode() for every individual instruction. The input to this function
// should not contain any semi-colons (";"), as that would denote multiple instructions.
//
// @param - input: The string containing all arguments for the instruction_t to be returned.
//
// @return - The instruction_t that was parsed from the input.
//
instruction_t decode_instruction(string input) {
  instruction_t instruction;
  string output_file;
  vector<command_t> cmds;
  vector<string> args;
  // Get background symbol
  vector<string> sep_by_background;
  if ((sep_by_background = separate(input, SYMBOL_BACKGROUND)).back() != input) {
    instruction.is_foreground = false;
    input = sep_by_background.front();
  }  
  // Decode commands
  vector<string> command_strings = separate(input, SYMBOL_PIPE);
  for (string cmd_str : command_strings) {
    args = separate(cmd_str, SYMBOL_ARG_DELIMITER);
    while (cmd_str == command_strings.back() && args.size() >= 3) { // Get input/output files
      string write_symbol = args.at(args.size() - 2);
      if (write_symbol == SYMBOL_OUT_REDIRECT_TRUNCATE) {
	instruction.write_mode = WRITE_MODE_TRUNCATE;
	instruction.output_file = args.back();
      } else if (write_symbol == SYMBOL_OUT_REDIRECT_APPEND) {
	instruction.write_mode = WRITE_MODE_APPEND;
	instruction.output_file = args.back();
      } else if (write_symbol == SYMBOL_IN_REDIRECT) {
	instruction.input_file = args.back();
      } else {
	break;
      }
      args.pop_back();
      args.pop_back();
    }
    cmds.push_back(command_t(args));
  }
  instruction.cmds = cmds;
  return instruction;
}

//
// Parses instructions separated by semi-colons (';') in the given string.
// If there are no semi-colons, the vector will contain just one instruction.
//
// @Param - input: the raw text containing command args separated by spaces (' ').
//
// @Return - A vector containing the instructions parsed from the string.
//
vector<instruction_t> decode(string input) {
  vector<instruction_t> instructions;
  vector<string> instruction_strings = separate(input, SYMBOL_INSTRUCTION_DELIMITER);
  for (string str : instruction_strings) {
    instructions.push_back(decode_instruction(str));
  }
  return instructions;
}
