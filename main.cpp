#include<iostream>
#include<string>
#include<vector>

#include<unistd.h>

#include<readline/readline.h>
#include<readline/history.h>

using namespace std;

#define MAX_ARGS 20
#define PROMPT_TEXT ">>> "

// - Commands

// - I/O Utils

string get_input() {
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));
  strcat(cwd, PROMPT_TEXT);
  return readline(cwd);
}

vector<vector<string> > parse_cmds(string input) {
  vector<vector<string> > cmds;
  char *input_str = new char[input.length() - 1];
  strcpy(input_str, input.c_str());
  char *cmd_str, *arg_str;
  while ((cmd_str = strsep(&input_str, "|")) != NULL) {
    if (strlen(cmd_str) == 0) {
      continue;
    }
    vector<string> cmd;
    while ((arg_str = strsep(&cmd_str, " ")) != NULL) {
      if (strlen(arg_str) != 0) {
	cmd.push_back(arg_str);
      }
    }
    if (cmd.size() != 0) {
      cmds.push_back(cmd);
    }
  }
  return cmds;
}

// NOTE: Does not create new process
int execute(vector<string> cmd) {
  char *args[cmd.size() + 1];
  args[cmd.size()] = NULL;
  for (int i = 0; i < cmd.size(); i++) {
    args[i] = new char[cmd[i].length() + 1];
    strcpy(args[i], cmd[i].c_str());
  }
  return execvp(args[0], args);
}

void execute_cmd(vector<string> cmd) {
  pid_t pid = fork();
  if (pid < 0) {
    perror("Failed to create child process.");
    exit(1);
  } else if (pid == 0) {
    if (execute(cmd) < 0) {
      printf("Bad instruction\n");
      exit(0);
    }
  } else {
    wait(NULL);
  }
}

void execute_pipeline(vector<vector<string> > cmds) {
  int fd[2];
  pid_t pid;
  int fdd = 0;
  for (vector<vector<string> >::iterator cmd_it = cmds.begin(); cmd_it != cmds.end(); cmd_it++) {
    pipe(fd);
    if ((pid = fork()) < 0) {
      perror("fork");
      exit(1);
    } else if (pid == 0) {
      dup2(fdd, 0);
      if (cmd_it + 1 != cmds.end()) {
	dup2(fd[1], 1);
      }
      close(fd[0]);
      execute_cmd(*cmd_it);
      exit(1);
    } else {
      wait(NULL);
      close(fd[1]);
      fdd = fd[0];
    }
  }
}

void execute(vector<vector<string> > cmds) {
  if (cmds.size() > 1) {
    execute_pipeline(cmds);
  } else {
    execute_cmd(cmds[0]);
  }
}

// - Main

void print_cmds(vector<vector<string> > cmds) {
  for (vector<vector<string> >::iterator cmd_it = cmds.begin(); cmd_it != cmds.end(); cmd_it++) {
    for (vector<string>::iterator arg_it = cmd_it->begin(); arg_it != cmd_it->end(); arg_it++) {
      cout << *arg_it << ' ';
    }
    cout << '.' << endl;
  }
}

int main(int argc, char *argv[]) {

  string input;
  vector<vector<string> > cmds;

  while (1) {
    input = get_input();

    add_history(input.c_str()); // TODO: Make wrapper method for this that takes a std::string
    
    cmds = parse_cmds(input);

    //print_cmds(cmds);
    execute_pipeline(cmds);
    
    // TODO: Re-implement the semi-colon shit
  }

  return 0;
}
