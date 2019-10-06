#include"io.h"
#include"model.h"
#include"decoder.h"
#include"executor.h"

#include<iostream>
#include<string>
#include<vector>

using namespace std;

int main(int argc, char *argv[]) {

  string input;
  vector<instruction_t> instrs;

  while (1) {
    input = get_input();

    add_to_history(input);

    instrs = decode(input);

    for (vector<instruction_t>::iterator instr_it = instrs.begin(); instr_it != instrs.end(); instr_it++) {
      execute(*instr_it);
    }
  }

  return 0;
}
