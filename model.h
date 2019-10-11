/**
   
   model.h

   This module contains the models command_t and instruction_t. A command_t consists
   of a single program and its arguments. An instruction_t consists of I/O redirection
   information, background/foreground execution information, and multiple command_t's
   (or just one if there are no pipes). Additionally, all instruction/command execution
   is encapsulated in this module.
   
 */

#pragma once

#include<vector>
#include<string>

//
// This is a model for a single command. That is, a single program and its arguments
// (e.g., "ls -la"). A command_t is responsible for executing itself and creating
// child processes as needed.
//
class command_t {

 public:

  std::vector<std::string> args;
  bool is_built_in = true; // TODO: Encapsulation


  //
  // Constructor that determines whether this command_t is built-in.
  // If a non-built-in command has a built-in counterpart, this constructor will
  // change the command's name.
  //
  // @param args: A vector containing the command arguments.
  //
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
  //        error_fd: The file descriptor to replace stderr.
  //
  // @Return 0 if the command executed successfully.
  //
  int exec(int input_fd, int output_fd, int error_fd);

 private:

  //
  // This is called by exec() if the command_t is built-in.
  //
  // The parameters and return value are the same as with exec().
  //
  int exec_built_in_cmd(int input_fd, int output_fd, int error_fd);

  //
  // This is called by exec() if the command is not built-in.
  //
  // @return 0 if the command executed successfully.
  //
  int exec_special_cmd();

};

//
// An enumeration to determine whether an instruction_t will truncate to a file (">"),
// append to it (">>"), or not redirect its output at all.
// 
enum write_mode_t {
  WRITE_MODE_NONE,
  WRITE_MODE_TRUNCATE,
  WRITE_MODE_APPEND
};

//
// This is the top level model for commands. An instruction_t consists of multiple
// command_t's (or just one if there's no piping) and handles the execution and I/O
// redirection of its commands.
//
class instruction_t {

 public:

  std::vector<command_t> cmds;
  bool is_foreground = true;
  std::string input_file;
  std::string output_file;
  write_mode_t write_mode = WRITE_MODE_NONE;
  
  //
  // This executes the instruction_t, handling pipeing and I/O redirection as
  // needed. If the instruction_t is set to execute in the background, this
  // method will fork a new child process and return immediately.
  //
  // @return - 0 if there were no errors.
  //
  int exec();

 private:

  //
  // This is called by exec() if the instruction_t contains multiple piped
  // command_t's (e.g., "ls -l | sort").
  //
  // @return - 0 if there were no errors.
  //
  int execute_instruction_with_pipes();

  //
  // This is called by exec() if the instruction_t contains only one command
  // with no pipes (e.g., "ls -l").
  //
  // @return - 0 if there were no errors.
  //
  int execute_instruction_no_pipes();

};
