#pragma once

#include<vector>
#include<string>

typedef std::vector<std::string> command_t; // A single command to be executed by 'exec'
typedef std::vector<command_t> instruction_t; // A pipeline of commands
