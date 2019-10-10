#pragma once
#include<iostream>
#include<fstream>

//
// Displays command prompt line containing current directory and waits
// for user to press 'return'.
// @Return - the string entered by the user
//
std::string get_input();

//
// A wrapper class for opening and iterating through the lines of an ASCII file
//
class batch_file {

 public:

  //
  // Constructor that opens an ifstream with the specified file
  // @Param - file: name of the file to be opened
  //
  batch_file(std::string file);

  //
  // Destructor that closes the file
  //
  ~batch_file();

  //
  // Returns a line from the file and iterates to the following file
  // @Return - the next line of the file that hasn't been returned already
  //
  std::string get_next_line();

  //
  // @Return - true if there are no more lines to return
  //
  bool is_at_end();

 private:

  std::ifstream instream;

};
