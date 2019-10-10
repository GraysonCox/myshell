#pragma once

#include<vector>
#include<string>

//
// This is a model for a single command. That is, a single program and its arguments.
// // TODO: Comments
//
class command_t {

 public:

  std::vector<std::string> args;

  bool is_built_in = true; // TODO: Encapsulation

  command_t(std::vector<std::string> args);

  //
  // The command executes itself. If the particular command is built-in, this function 
  // will fork a child process which then calls execvp(). If the command isn't built-in,
  // no child process is forked. This encapsulates all forking at the command level.
  //
  // @Param input_fd: The input file descriptor that will replace stdin within the child
  //                  process. If no input redirection or piping is desired, this should 
  //                  be stdin.
  //       output_fd: The output file descriptor that will replace stdout within the child
  //                  process. If no output redirection or piping is desired, this should 
  //                  be stdout.
  //
  // @Return 0 if the command executed successfully.
  //
  int exec(int input_fd, int output_fd, int error_fd);

 private:

  // TODO: Comments
  int exec_built_in_cmd(int input_fd, int output_fd, int error_fd);

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
