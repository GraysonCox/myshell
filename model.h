#pragma once

#include<vector>
#include<string>

//typedef std::vector<std::string> command_t; // A single command to be executed by 'exec'
//typedef std::vector<command_t> instruction_t; // A pipeline of commands

class command_t {

 public:

  std::vector<std::string> args;

  bool is_built_in = true;

  command_t(std::vector<std::string> args);

  int exec();

};

enum write_mode_t {
  WRITE_MODE_NONE,
  WRITE_MODE_TRUNCATE,
  WRITE_MODE_APPEND
};

class instruction_t {

 public:

  std::vector<command_t> cmds; // TODO: Make this private.

  bool is_foreground;

  std::string output_file;

  write_mode_t write_mode;

  instruction_t(std::vector<command_t> cmds, bool is_foreground = true, std::string output_file = "", write_mode_t write_mode = WRITE_MODE_NONE);

  int exec();

 private:

  int execute_instruction_with_pipes();

  int execute_instruction_no_pipes();

};
