#include"io.h"
#include"model.h"
#include"decoder.h"

#include<iostream>
#include<string>
#include<vector>

using namespace std;

int main(int argc, char *argv[]) {

  string input;
  vector<instruction_t> instructions;

  while (1) {
    input = get_input();

    //cout << "Input: " << input << endl;

    add_to_history(input);

    instructions = decode(input);

    for (instruction_t instr : instructions) {
      instr.exec();
    }
  }

  return 0;
}
