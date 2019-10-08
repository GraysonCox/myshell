#pragma once
#include<iostream>
#include<fstream>

std::string get_input();

void add_to_history(std::string s);

class batch_file {

 public:

  batch_file(std::string file);

  ~batch_file();

  std::string get_next_line();

  bool is_at_end();

 private:

  std::ifstream instream;

};
