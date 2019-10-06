#include"io.h"

#include<unistd.h>

#include<readline/readline.h>
#include<readline/history.h>

std::string get_input() {
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));
  strcat(cwd, PROMPT_TEXT.c_str());
  return readline(cwd);
}

void add_to_history(std::string s) {
  add_history(s.c_str());
}
