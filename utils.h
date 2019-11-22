/*
*   utils.h
*   part of image blur software using CUDA
*   for CSC 630 with Dr. Zhang
*   Created by Dan McGonigle, 11/21/2019
*
*   This header file contains the definitions for utility functions used by the blur software.
*/

#include <chrono>
#include <string>
#include <vector>

//  Print verbose messages during program runtime
void debug(std::string str, bool debugFlag);

//  Cast time to string to display program runtime
std::string timePointAsString(const std::chrono::system_clock::time_point& tp);

//   Check to see if file exists
bool fileExists (const std::string& path);

//  Split string into vector of strings any character delimiter
std::vector<std::string> split(const std::string& s, char delimiter);

//  Split string into vector of strings space delimiter
std::vector<std::string> split(const std::string& s);
