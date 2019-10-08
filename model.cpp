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

int command_t::exec() {
  if (is_built_in) {  
    char *arg_array[args.size() + 1];
    arg_array[args.size()] = NULL;
    for (int i = 0; i < args.size(); i++) {
      arg_array[i] = new char[args.at(i).length() + 1];
      strcpy(arg_array[i], args.at(i).c_str());
    }
    
    return execvp(arg_array[0], arg_array);
  }
  string cmd_name = args.at(0);
  if (cmd_name == "cd") {
    return chdir(args.at(1).c_str());
  } if (cmd_name == "help") {
    system("help | more");
    return 0;
  } 
  exit(0);
}

// - instruction_t

instruction_t::instruction_t(vector<command_t> cmds, bool is_foreground, string output_file, write_mode_t write_mode) {
  this->cmds = cmds;
  this->is_foreground = is_foreground;
  this->output_file = output_file;
  this->write_mode = write_mode;
}

int instruction_t::execute_instruction_no_pipes() {
  if (!cmds.front().is_built_in) {
    return cmds.front().exec();
  }

  pid_t pid = fork();
  if (pid < 0) {
    perror("Failed to create child process.");
    exit(1);
  } else if (pid == 0) {
    if (cmds.at(0).exec() < 0) {
      printf("Bad instruction\n");
      exit(2);
    }
    exit(0); // TODO: Wuh...
  } else if (is_foreground) {
    waitpid(pid, NULL, 0);
  }
  return 0; // TODO: Return -1 if something goes wrong. This always returns 0;
}

int instruction_t::execute_instruction_with_pipes() {
  int fd[2];
  pid_t pid;
  int fdd = 0;
  for (vector<command_t>::iterator cmd_it = cmds.begin(); cmd_it != cmds.end(); cmd_it++) {
    pipe(fd); // TODO: Use pipes only if needed.
    if ((pid = fork()) < 0) {
      perror("Failed to create child process.");
      exit(1);
    } else if (pid == 0) {
      dup2(fdd, 0);
      if (cmd_it + 1 != cmds.end()) {
        dup2(fd[1], 1);
      }
      close(fd[0]);
      cmd_it->exec();
      exit(1);
    } else if (is_foreground || cmd_it + 1 != cmds.end()) {
      waitpid(pid, NULL, 0);
    }
    close(fd[1]);
    fdd = fd[0];
  }
  return 0; // TODO: Return -1 if something goes wrong. This always returns 0;
}

int instruction_t::exec() {
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
    if (-1 == out) { perror("opening output file"); return 255; }
    save_out = dup(fileno(stdout));
    if (-1 == dup2(out, fileno(stdout))) { perror("cannot redirect stdout"); return 255; }
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
  return error_code;
}
