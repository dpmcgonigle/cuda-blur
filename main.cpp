/*
*   main.cpp
*   part of image blur software using CUDA
*   for CSC 630 with Dr. Zhang
*   Created by Dan McGonigle, 11/21/2019
*
*   This program takes an input image and performs a blur operation on it, saving the output
*
*   This software uses the boost library with program_options for command-line parsing.
*   Command-line arguments:
*         option            input           description
*       --debug, -d         none            boolean flag for verbose print statements
*       --input, -i         input path      specify the image path for the image to blur
*       --output, -o        output path     specify the image path for the blurred image
*       --filtersize, -f    filter size     1 for 3x3, 2 for 5x5, 3 for 7x7
*       --cuda              none            boolean flag for using cuda vs cpu
*       --help, -h          none            display help for this program
*
*   Compiling the program:
*       Simply use the make file
*
*   Running the program:
*       ./blur.exe --debug --input img/mountain.jpg --filtersize 2
*/

#define cimg_display 0

#include "boost/program_options.hpp" 
#include "utils.h"
#include "cimg_utils.h"
#include "CImg.h"
#include <iostream> 
#include <string> 
#include <chrono>
 
namespace 
{ 
    const size_t ERROR_IN_COMMAND_LINE = 1; 
    const size_t SUCCESS = 0; 
    const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
} // namespace 

namespace cl=cimg_library;
 
int main(int argc, char** argv) 
{ 
    try 
    { 
        /*
        *   Define program options 
        */ 
        bool debugFlag=false;
        bool cudaFlag=false;
        std::string inputPath;
        std::string outputPath;
        int filterSize;
        namespace po = boost::program_options; 
        po::options_description desc("Options"); 
        desc.add_options() 
            ("help,h", "Print help messages") 
            ("input,i", po::value(&inputPath), "Path of the image to blur (REQUIRED).")
            ("output,o", po::value(&outputPath), "Path of the resulting output.")
            ("filtersize,f", po::value(&filterSize) -> default_value(1), "Filter size. 1 => 3x3, 2 => 5x5, 3 => 7x7, etc.")
            ("cuda,c", po::bool_switch(&cudaFlag), "Perform blur operation on CUDA. Otherwise perform sequentially on single CPU.")
            ("debug,d", po::bool_switch(&debugFlag), "Enable verbose debugging statements."); 
 
        po::variables_map vm; 
    try 
    { 
        /*
        *   Parse program options
        */ 
        po::store(po::parse_command_line(argc, argv, desc), vm); // can throw 
 
        //  Help
        if ( vm.count("help")  ) 
        { 
            std::cout << std::endl << "\t~\t~\t~\tblur.exe\t~\t~\t~\t~" << std::endl << std::endl
            << "\tThis program takes an input image and blurs it using CUDA." << std::endl
            << "\tExample: ./blur.exe --debug --input img/mountain.jpg" << std::endl << std::endl
            << desc << std::endl; 
            return SUCCESS; 
        } 
        po::notify(vm); // throws on error

        //  input image
        if ( inputPath.empty() )
        {
            std::cerr << "ERROR: Input path for image is empty. Exit with code " << ERROR_IN_COMMAND_LINE << "." << std::endl;
            return ERROR_IN_COMMAND_LINE;
        }
        else if ( !fileExists(inputPath) )
        {
            std::cerr << "ERROR: Input path " << inputPath << " doesn't exist. Exit with code " << ERROR_IN_COMMAND_LINE << "." << std::endl;
            return ERROR_IN_COMMAND_LINE;
        }
        debug("INPUT PATH: " + inputPath , debugFlag);

        //  output image
        if ( outputPath.empty() )
        {
            std::vector<std::string> pathTokens = split(inputPath, '.');
            outputPath = "";
            for (int i = 0; (unsigned)i < pathTokens.size()-1; i++ )
            {
                outputPath += pathTokens.at(i);
            }
            outputPath += "_blur." + pathTokens.at(pathTokens.size() - 1);
        }
        debug("OUTPUT PATH: " + outputPath , debugFlag);
    } 
    catch(po::error& e) 
    { 
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
        std::cerr << desc << std::endl; 
        return ERROR_IN_COMMAND_LINE; 
    } 
 
    // application code here // 
    debug("Program start", debugFlag);
    cl::CImg<unsigned char> image(inputPath.c_str());
    debug("CImg width: " + std::to_string( image.width() ) , debugFlag );;
    debug("CImg height: " + std::to_string( image.height() ) , debugFlag );;
    debug("CImg channels: " + std::to_string( image.spectrum() ) , debugFlag );


    //  Only the blurring operation should be timed
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    image = blur(image, filterSize, cudaFlag);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    //  Save image
    image.save(outputPath.c_str());

    debug("Program end \nBlurring Runtime: " 
        + std::to_string( std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() ) + "[µs], or " +
        std::to_string( std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() ) + "[ns]", debugFlag);
  } 
  catch(std::exception& e) 
  { 
    std::cerr << "Unhandled Exception reached the top of main: " 
              << e.what() << ", application will now exit" << std::endl; 
    return ERROR_UNHANDLED_EXCEPTION; 
 
  } 
 
  return SUCCESS; 
 
} // main 

