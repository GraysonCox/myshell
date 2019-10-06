#pragma once
#include<iostream>

const std::string PROMPT_TEXT = ">>> ";

std::string get_input();

void add_to_history(std::string s);
