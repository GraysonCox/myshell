#pragma once

#include"model.h"

#include<vector>
#include<string>

//
// Parses instructions separated by semi-colons (';') in the given string.
// If there are no semi-colons, the vector will contain just one instruction.
// @Param - input: the raw text containing command args separated by spaces (' ').
// @Return - A vector containing the instructions parsed from the string.
//
std::vector<instruction_t> decode(std::string input);
