/**
   
   model.cpp

   This module contains the models command_t and instruction_t. A command_t consists
   of a single program and its arguments. An instruction_t consists of I/O redirection
   information, background/foreground execution information, and multiple command_t's
   (or just one if there are no pipes). Additionally, all instruction/command execution
   is encapsulated in this module.
   
 */

#include"model.h"

#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<cstring>
#include<stdio.h>
#include<string.h>
#include<iostream> // TODO: Delete

using namespace std;

extern char** environ;

// - command_t

//
// Constructor that determines whether this command_t is built-in.
// If a non-built-in command has a built-in counterpart, this constructor will 
// change the command's name.
//
// @param args: A vector containing the command arguments.
//
command_t::command_t(vector<string> args) {
  this->args = args;
  string cmd_name = args.front();
  if (
      cmd_name == "cd"
      || cmd_name == "help"
      || cmd_name == "quit"
      || cmd_name == "pause"
      || cmd_name == "env"
  ) {
    this->is_built_in = false;
  } else if (cmd_name == "environ") {
    this->args.erase(this->args.begin());
    this->args.emplace(this->args.begin(), "env");
    this->is_built_in = false;
  } else if (cmd_name == "clr") {
    this->args.erase(this->args.begin());
    this->args.emplace(this->args.begin(), "clear");
  } else if (cmd_name == "dir") {
    this->args.erase(this->args.begin());
    this->args.emplace(this->args.begin(), "ls");
  }
}

//
// This is called by exec() if the command_t is built-in.
//
// The parameters and return value are the same as with exec().
//
int command_t::exec_built_in_cmd(int input_fd, int output_fd, int error_fd) {
  pid_t pid;
  if ((pid = fork()) < 0) {
    perror("Bad fork");
    return -1;
  } else if (pid > 0) {
    return waitpid(pid, NULL, 0);
  } else {
    dup2(input_fd, 0);
    dup2(output_fd, 1);
    dup2(error_fd, 2);
    char *arg_array[args.size() + 1]; // TODO: Make separate method for making this array.
    arg_array[args.size()] = NULL;
    for (size_t i = 0; i < args.size(); i++) {
      arg_array[i] = new char[args.at(i).length() + 1];
      strcpy(arg_array[i], args.at(i).c_str());
    }
    execvp(arg_array[0], arg_array);
    fputs("Invalid instruction.\n", stderr);
    exit(-1);
  }
}

//
// This is called by exec() if the command is not built-in.
//
// @return 0 if the command executed successfully.
//
int command_t::exec_special_cmd() {
  string cmd_name = args.at(0);
  if (cmd_name == "cd") {
    if (args.size() > 1) {
      if (chdir(args.at(1).c_str()) == -1) {
	perror(args.at(1).c_str());
	return -1;
      } else {
	return 0;
      }
    } else {
      system("pwd");
      return 0;
    }
  } else if (cmd_name == "help") {
    system("help | more");
  } else if (cmd_name == "quit") {
    exit(0);
  } else if (cmd_name == "pause") {
    string s;
    getline(cin, s);
  } else if (cmd_name == "env") {
    for (size_t i = 0; environ[i] != NULL; i++) {
      printf("%s\n", environ[i]);
    }
  }
  return 0;
}

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
int command_t::exec(int input_fd, int output_fd, int error_fd) {
  if (is_built_in) {
    return exec_built_in_cmd(input_fd, output_fd, error_fd);
  } else {
    return exec_special_cmd();
  }
}

// - instruction_t

//
// This is called by exec() if the instruction_t contains multiple piped
// command_t's (e.g., "ls -l | sort").
//
// @return - 0 if there were no errors.
//
int instruction_t::execute_instruction_no_pipes() {
  return cmds.front().exec(0, 1, 2);
}

//
// This is called by exec() if the instruction_t contains only one command
// with no pipes (e.g., "ls -l").
//
// @return - 0 if there were no errors.
//
int instruction_t::execute_instruction_with_pipes() {
  int fd[2];
  int fd_buf = 0;
  for (vector<command_t>::iterator cmd_it = cmds.begin(); cmd_it != cmds.end(); cmd_it++) {
    pipe(fd);
    if (cmd_it + 1 != cmds.end()) {
      cmd_it->exec(fd_buf, fd[1], fileno(stderr));
    } else {
      cmd_it->exec(fd_buf, 1, fileno(stderr));
    }
    close(fd[1]);
    fd_buf = fd[0];
  }
  return 0;
}

//
// This executes the instruction_t, handling pipeing and I/O redirection as
// needed. If the instruction_t is set to execute in the background, this
// method will fork a new child process and return immediately.
//
// @return - 0 if there were no errors.
//
int instruction_t::exec() {
  pid_t pid = -1;
  if (!is_foreground) { // If this should be in the background, make a new child process so that the parent can return immediately.
    if ((pid = fork()) < 0) {
      perror("Bad fork");
      return -1;
    } else if (pid > 0) {
      return 0;
    }
  }
  int in, out, save_in, save_out, save_err; // File descriptors for I/O redirection
  bool is_output_redirected = !output_file.empty();
  bool is_input_redirected = !input_file.empty();
  if (is_input_redirected) { // Open input file if redirected
    in = open(input_file.c_str(), O_RDONLY);
    if (in == -1) { perror("Failed to open input file"); exit(-1); }
    save_in = dup(fileno(stdin));
    if (dup2(in, fileno(stdin)) == -1) { perror("Failed to redirect stdin."); exit(-1); }
  }
  if (is_output_redirected) { // Open output and error file if redirected
    int write_code = 0;
    switch (write_mode) {
    case WRITE_MODE_TRUNCATE:
      write_code = O_RDWR|O_CREAT|O_TRUNC;
      break;
    case WRITE_MODE_APPEND:
      write_code = O_RDWR|O_CREAT|O_APPEND;
      break;
    default:
      break;
    }
    out = open(output_file.c_str(), write_code, 0600);
    if (-1 == out) { perror("Failed to open output file."); exit(-1); }
    save_out = dup(fileno(stdout));
    save_err = dup(fileno(stderr));
    if (-1 == dup2(out, fileno(stdout))) { perror("Failed to redirect stdout."); exit(-1); }
    if (-1 == dup2(out, fileno(stderr))) { perror("Failed to redirect stderr."); exit(-1); }
  }
  int error_code = 0;
  if (cmds.size() > 1) { // Execute instruction
    error_code = execute_instruction_with_pipes();
  } else {
    error_code = execute_instruction_no_pipes();
  }
  if (is_input_redirected) { // Close input file if redirected
    fflush(stdin);
    close(in);
    dup2(save_in, fileno(stdin));
    close(save_in);
  }
  if (is_output_redirected) { // Close output file if redirected
    fflush(stdout);
    fflush(stderr);
    close(out);
    dup2(save_out, fileno(stdout));
    dup2(save_err, fileno(stderr));
    close(save_out);
    close(save_err);
  }
  if (pid == 0) { 
    exit(error_code); // Terminate if this is a child running in the background.
  } else {
    return 0; // Return if this is the parent process
  }
}
