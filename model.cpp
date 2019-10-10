#include"model.h"

#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<cstring>

#include<iostream> // TODO: Remove when done debugging

using namespace std;

// - command_t

command_t::command_t(vector<string> args) {
  this->args = args;
  string cmd_name = args.front();
  if (
      cmd_name == "cd"
      || cmd_name == "help"
      || cmd_name == "quit"
      || cmd_name == "echo"
  ) {
    this->is_built_in = false;
  } else if (cmd_name == "environ") {
    this->args.erase(this->args.begin());
    this->args.emplace(this->args.begin(), "env");
  } else if (cmd_name == "clr") {
    this->args.erase(this->args.begin());
    this->args.emplace(this->args.begin(), "clear");
  } else if (cmd_name == "dir") {
    this->args.erase(this->args.begin());
    this->args.emplace(this->args.begin(), "ls");
  }
}

int command_t::exec_built_in_cmd(int input_fd, int output_fd) {
  pid_t pid;
  if ((pid = fork()) < 0) {
    cerr << "Bad fork" << endl;
    return -1;
  } else if (pid > 0) {
    return waitpid(pid, NULL, 0);
  } else {
    dup2(input_fd, 0);
    dup2(output_fd, 1);
    char *arg_array[args.size() + 1]; // TODO: Make separate method for making this array.
    arg_array[args.size()] = NULL;
    for (size_t i = 0; i < args.size(); i++) {
      arg_array[i] = new char[args.at(i).length() + 1];
      strcpy(arg_array[i], args.at(i).c_str());
    }
    execvp(arg_array[0], arg_array);
    exit(-1);
  }
}

int command_t::exec_special_cmd() {
  string cmd_name = args.at(0);
  if (cmd_name == "cd") {
    if (args.size() > 1) {
      return chdir(args.at(1).c_str());
    } else {
      system("pwd");
      return 0;
    }
  } else if (cmd_name == "help") {
    system("help | more");
  } else if (cmd_name == "quit") {
    exit(0);
  } else if (cmd_name == "pause") {

  } else if (cmd_name == "echo") {
    for (auto str = args.begin() + 1; str != args.end(); str++) {
      cout << *str << ' ';
    }
    cout << endl;
  }
  return 0;
}

int command_t::exec(int input_fd, int output_fd) {
  if (is_built_in) {
    return exec_built_in_cmd(input_fd, output_fd);
  } else {
    return exec_special_cmd();
  }
}

// - instruction_t

/*instruction_t::instruction_t(vector<command_t> cmds) {
  this->cmds = cmds;
  }*/

int instruction_t::execute_instruction_no_pipes() {
  return cmds.front().exec(0, 1);
}

int instruction_t::execute_instruction_with_pipes() {
  int fd[2];
  int fdd = 0; // TODO: Rename this.
  for (vector<command_t>::iterator cmd_it = cmds.begin(); cmd_it != cmds.end(); cmd_it++) {
    pipe(fd);
    if (cmd_it + 1 != cmds.end()) {
      cmd_it->exec(fdd, fd[1]);
    } else {
      cmd_it->exec(fdd, 1);
    }
    close(fd[1]);
    fdd = fd[0];
    //close(fd[0]); // TODO: Maybe worry about pipe leaks
  }
  return 0; // TODO: Return -1 if something goes wrong. This always returns 0;
}

int instruction_t::exec() {
  pid_t pid = -1;
  if (!is_foreground) {
    if ((pid = fork()) < 0) {
      cerr << "Bad fork" << endl;
      return -1;
    } else if (pid > 0) {
      return 0;
    }
  }

  int in, out, save_in, save_out, save_err;
  bool is_output_redirected = !output_file.empty();
  bool is_input_redirected = !input_file.empty();
  if (is_input_redirected) {
    in = open(input_file.c_str(), O_RDONLY);
    if (in == -1) { perror("Failed to open input file"); exit(-1); }
    save_in = dup(fileno(stdin));
    if (dup2(in, fileno(stdin)) == -1) { perror("Failed to redirect stdin."); exit(-1); }
  }
  if (is_output_redirected) {
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
  if (cmds.size() > 1) {
    error_code = execute_instruction_with_pipes();
  } else {
    error_code = execute_instruction_no_pipes();
  }
  if (is_input_redirected) {
    fflush(stdin);
    close(in);
    dup2(save_in, fileno(stdin));
    close(save_in);
  }
  if (is_output_redirected) {
    fflush(stdout);
    fflush(stderr);
    close(out);
    dup2(save_out, fileno(stdout));
    dup2(save_err, fileno(stderr));
    close(save_out);
    close(save_err);
  }
  if (pid == 0) {
    exit(error_code);
  } else {
    return 0;
  }
}
