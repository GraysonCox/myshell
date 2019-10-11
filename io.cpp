/**

   io.cpp

   This module contains all the I/O that takes place in main.cpp (i.e., reading user
   input and parsing batch files).

 */

#include"io.h"

#include<iostream>
#include<cstring>
#include<unistd.h>

using namespace std;

const string PROMPT_TEXT = "$ ";

//
// Displays command prompt line containing current directory and waits
// for user to press 'return'.
// @Return - the string entered by the user
//
string get_input() {
  cin.clear();
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));
  strcat(cwd, PROMPT_TEXT.c_str());
  cout << cwd;
  string input;
  getline(cin, input);
  return input;
}

//
// Constructor that opens an ifstream with the specified file
// @Param - file: name of the file to be opened
//
batch_file::batch_file(string file) {
  instream.open(file);
}

//
// Destructor that closes the file
//
batch_file::~batch_file() {
  instream.close();
}

//
// Returns a line from the file and iterates to the following file
// @Return - the next line of the file that hasn't been returned already
//
string batch_file::get_next_line() {
  string str;
  getline(instream, str);
  return str;
}

//
// @Return - true if there are no more lines to return
//
bool batch_file::is_at_end() {
  return instream.eof();
}
