#include"io.h"

#include<iostream>
#include<cstring>
#include<unistd.h>

using namespace std;

const string PROMPT_TEXT = "$ ";

string get_input() {
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));
  strcat(cwd, PROMPT_TEXT.c_str());
  cout << cwd;
  string input;
  getline(cin, input);
  return input;
}

void add_to_history(string s) {
  //add_history(s.c_str());
}

batch_file::batch_file(string file) {
  instream.open(file);
}

batch_file::~batch_file() {
  instream.close();
}

string batch_file::get_next_line() {
  string str;
  getline(instream, str);
  return str;
}

bool batch_file::is_at_end() {
  return instream.eof();
}
