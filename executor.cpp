#include"executor.h"

#include"model.h"

#include<iostream>
#include<string>
#include<vector>
#include<unistd.h>

using namespace std;

static const string special_commands[] = {
  "cd",
  "help",
  "clr",
  "environ",
  "time",
  "dir",
  "quit"
};

// NOTE: Wrapper for execlp. Does not create new process.
int exec(command_t cmd) {
  string cmd_name = cmd[0];
  
  if (cmd_name == "cd") {
    return chdir(cmd[1].c_str());
  }

  char *args[cmd.size() + 1];
  args[cmd.size()] = NULL;
  for (int i = 0; i < cmd.size(); i++) {
    args[i] = new char[cmd[i].length() + 1];
    strcpy(args[i], cmd[i].c_str());
  }
  return execvp(args[0], args);
}

void execute_cmd(command_t cmd) {
  pid_t pid = fork();
  if (pid < 0) {
    perror("Failed to create child process.");
    exit(1);
  } else if (pid == 0) {
    if (exec(cmd) < 0) {
      printf("Bad instruction\n");
      exit(0);
    }
  } else {
    wait(NULL);
  }
}

void execute_pipeline(instruction_t cmds) {
  int fd[2];
  pid_t pid;
  int fdd = 0;
  for (instruction_t::iterator cmd_it = cmds.begin(); cmd_it != cmds.end(); cmd_it++) {
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
      exec(*cmd_it);
      exit(1);
    }
    wait(NULL);
    close(fd[1]);
    fdd = fd[0];
  }
}

void execute(instruction_t instr) {
  if (instr.size() > 1) {
    execute_pipeline(instr);
  } else {
    execute_cmd(instr.at(0));
  }
}
