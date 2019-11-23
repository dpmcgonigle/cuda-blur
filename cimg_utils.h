/*
*   cimg_utils.h
*   part of image blur software using CUDA
*   for CSC 630 with Dr. Zhang
*   Created by Dan McGonigle, 11/21/2019
*
*   This program contains CImg-manipulating function definitions for the image blur software
*/

#define cimg_OS 1
#define cimg_display 0
#include "CImg.h" 
#include <iostream> 
#include <vector>

namespace cl=cimg_library;
 
//  Blur original image
cl::CImg<unsigned char> blur( cl::CImg<unsigned char> image , int filterSize , bool cudaFlag );

//  Blur original image sequentially
cl::CImg<unsigned char> blur_sequential( cl::CImg<unsigned char> image , int filterSize );

//  Blur original image with cuda
cl::CImg<unsigned char> blur_cuda( cl::CImg<unsigned char> image , int filterSize );

//  Filter based on filterSize
std::vector<std::vector<float>> getFilter(int filterSize);
void getFilter(float **filter, int filterSize);

//  Print filter
void printFilter(std::vector<std::vector<float>> filter);
void printFilter(float **filter, int filterSize);
