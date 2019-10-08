#pragma once

#include<vector>
#include<string>

class command_t {

 public:

  std::vector<std::string> args;

  bool is_built_in = true;

  command_t(std::vector<std::string> args);

  int exec(int pipe[2], int input_fd, int output_fd);

 private:

  int exec_built_in_cmd();

  int exec_special_cmd();

};

enum write_mode_t {
  WRITE_MODE_NONE,
  WRITE_MODE_TRUNCATE,
  WRITE_MODE_APPEND
};

class instruction_t {

 public:

  std::vector<command_t> cmds;

  bool is_foreground = true;

  std::string input_file;

  std::string output_file;

  write_mode_t write_mode = WRITE_MODE_NONE;

  int exec();

 private:

  int execute_instruction_with_pipes();

  int execute_instruction_no_pipes();

};
