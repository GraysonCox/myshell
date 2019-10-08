#import"model.h"

#import<unistd.h>
#import<stdlib.h>
#import<fcntl.h>

#import<iostream> // TODO: Remove when done debugging

using namespace std;

// - command_t

command_t::command_t(vector<string> args) {
  this->args = args;
  string cmd_name = args.front();
  if (
      cmd_name == "cd"
      || cmd_name == "help"
      || cmd_name == "quit"
  ) {
    this->is_built_in = false;
  }
}

int command_t::exec_built_in_cmd() {
  char *arg_array[args.size() + 1];
  arg_array[args.size()] = NULL;
  for (int i = 0; i < args.size(); i++) {
    arg_array[i] = new char[args.at(i).length() + 1];
    strcpy(arg_array[i], args.at(i).c_str());
  }
  return execvp(arg_array[0], arg_array);
}

int command_t::exec_special_cmd() {
  string cmd_name = args.at(0);
  if (cmd_name == "cd") {
    return chdir(args.at(1).c_str());
  } else if (cmd_name == "help") {
    system("help | more");
  } else if (cmd_name == "quit") {
    exit(0);
  }
  return 0;
}

int command_t::exec(int pipe[2], int input_fd, int output_fd) {
  if (!is_built_in) {
    return exec_special_cmd();
  } else {
    pid_t pid;
    if ((pid = fork()) < 0) {
      cerr << "Bad fork" << endl;
      return -1;
    } else if (pid > 0) {
      waitpid(pid, NULL, 0);
      return 0;
    } else {
      dup2(input_fd, 0);
      dup2(output_fd, 1);
      //close(pipe[0]);
      return exec_built_in_cmd();
    }
  }
}

// - instruction_t

/*instruction_t::instruction_t(vector<command_t> cmds) {
  this->cmds = cmds;
  }*/

int instruction_t::execute_instruction_no_pipes() {
  cmds.front().exec(NULL, 0, 1);
  return 0; // TODO: Return -1 if something goes wrong. This always returns 0;
}

int instruction_t::execute_instruction_with_pipes() {
  int fd[2];
  //pid_t pid;
  int fdd = 0;
  for (vector<command_t>::iterator cmd_it = cmds.begin(); cmd_it != cmds.end(); cmd_it++) {
    pipe(fd);
    if (cmd_it + 1 != cmds.end()) {
      cmd_it->exec(fd, fdd, fd[1]);
    } else {
      cmd_it->exec(fd, fdd, 1);
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

  int out, save_out;
  bool is_output_redirected = output_file.length() != 0;
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
    if (-1 == out) { perror("opening output file"); exit(-1); }
    save_out = dup(fileno(stdout));
    if (-1 == dup2(out, fileno(stdout))) { perror("cannot redirect stdout"); exit(-1); }
  }
  int error_code = 0;
  if (cmds.size() > 1) {
    error_code = execute_instruction_with_pipes();
  } else {
    error_code = execute_instruction_no_pipes();
  }
  if (is_output_redirected) {
    fflush(stdout); close(out);
    dup2(save_out, fileno(stdout));
    close(save_out);
  }
  if (pid == 0) {
    exit(0);
  } else {
    return 0;
  }
}
