/*
*   utils.cpp
*   part of image blur software using CUDA
*   for CSC 630 with Dr. Zhang
*   Created by Dan McGonigle, 11/21/2019
*
*   This file contains the implementation for functions used in the image blur software.
*/

#include "utils.h"
#include <iostream> 
#include <sstream> 
#include <string> 
#include <chrono>
#include <vector>
#include <sys/stat.h>
 
/*
*   Print verbose logging through program runtime.
*   Only prints if debugFlag is set, which can be done at command-line.
*/
void debug(std::string str, bool debugFlag)
{
    if (debugFlag)
    {
        std::cout << "DEBUG: " << str << std::endl;
    }
} 

/*
*   Returns a string representation of a chrono::time_point
*/
std::string timePointAsString(const std::chrono::system_clock::time_point& tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::string ts = std::ctime(&t);
    ts.resize(ts.size()-1);
    return ts;
}

bool fileExists (const std::string& path) {
    struct stat buffer;   
    return (stat (path.c_str(), &buffer) == 0); 
}

/*
*   Split a string and return a vector of string tokens.
*   Delimiter can by any single character.
*/
std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

/*
*   Split a string and return a vector of string tokens.
*   Delimiter defaults to space.
*/
std::vector<std::string> split(const std::string& s)
{
    char delimiter = ' ';
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}
