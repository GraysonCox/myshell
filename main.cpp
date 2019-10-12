/**

   main.cpp

   Contains main method and what not.

 */

#include"io.h"
#include"model.h"
#include"decoder.h"

#include<iostream>
#include<string>
#include<vector>
#include<unistd.h>
#include<stdlib.h>
#include<cstring>

using namespace std;

//
// Adds SHELL=(directory path)/myshell to the list of environment variables.
//
void add_path_variable() {
  string path_var;
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));
  path_var.append(string(cwd));
  path_var.append("/myshell");
  setenv("SHELL", path_var.c_str(), 1);
}

//
// Continuously take user input via the keyboard, decode the input as
// instruction_t's, and execute them. Exits when the model.h module
// exits.
//
void do_command_line() {
  string input;
  vector<instruction_t> instructions;
  while (1) {
    input = get_input();
    instructions = decode(input);
    for (instruction_t instr : instructions) {
      instr.exec();
    }
  }
}

void do_batch_file(string file) {
  batch_file f(file);
  string input;
  vector<instruction_t> instructions;
  while (!f.is_at_end()) {
    input = f.get_next_line();
    instructions = decode(input);
    for (instruction_t instr : instructions) {
      instr.exec();
    }
  }
}

int main(int argc, char *argv[]) {

  add_path_variable();

  if (argc > 1) { // TODO: Better error handling
    do_batch_file(argv[1]);
  } else {
    do_command_line();
  }

  return 0;
}
