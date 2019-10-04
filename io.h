#pragma once
#include<iostream>

namespace io {

  const std::string PROMPT_TEXT = ">>> ";

  std::string get_input();

  void add_to_history(std::string s);

}
