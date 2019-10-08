#include"io.h"
#include"model.h"
#include"decoder.h"

#include<iostream>
#include<string>
#include<vector>

using namespace std;

void do_command_line() {
  string input;
  vector<instruction_t> instructions;
  while (1) {
    input = get_input();
    add_to_history(input);
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

  if (argc > 1) { // TODO: Better error handling
    do_batch_file(argv[1]);
  } else {
    do_command_line();
  }

  return 0;
}
