/*
*   cimg.cpp
*   part of image blur software using CUDA
*   for CSC 630 with Dr. Zhang
*   Created by Dan McGonigle, 11/21/2019
*
*   This program implements CImg functionality for the image blur software
*/

#define cimg_OS 1
#define cimg_display 0
#include "CImg.h" 
#include "cimg_utils.h"
#include <iostream> 
#include <stdlib.h>
#include <vector>

//namespace cl=cimg_library;
namespace cl=cimg_library;
 
//  Blur
cl::CImg<unsigned char> blur( cl::CImg<unsigned char> image , int filterSize , bool cudaFlag )
{ 
    /*
    cimg_forX(image,x) 
    {
        image(x,80,0,0)=255; 
    }
    */
    return blur_sequential(image, filterSize, cudaFlag);
}

//  Sequential blur
cl::CImg<unsigned char> blur_sequential( cl::CImg<unsigned char> image , int filterSize , bool cudaFlag)
{
    std::vector<std::vector<float>> filter = getFilter(filterSize);
    printFilter(filter);
    return image;
}

//  Filter
std::vector<std::vector<float>> getFilter(int filterSize)
{
    //  FILTERS for size 1 through 3
    //  Obtained through http://dev.theomader.com/gaussian-kernel-calculator/
    std::vector<std::vector<float>> filter;

    switch(filterSize)
    {
        case 1:
            filter.insert(filter.end(), std::vector<float> {0.077847,    0.123317,   0.077847});
            filter.insert(filter.end(), std::vector<float> {0.123317,    0.195346,   0.123317});
            filter.insert(filter.end(), std::vector<float> {0.077847,    0.123317,   0.077847});
            return filter;
        break;

        case 2:
            filter.insert(filter.end(), std::vector<float> {0.003765,  0.015019,   0.023792,   0.015019,   0.003765});
            filter.insert(filter.end(), std::vector<float> {0.015019,  0.059912,   0.094907,   0.059912,   0.015019});
            filter.insert(filter.end(), std::vector<float> {0.023792,  0.094907,   0.150342,   0.094907,   0.023792});
            filter.insert(filter.end(), std::vector<float> {0.015019,  0.059912,   0.094907,   0.059912,   0.015019});
            filter.insert(filter.end(), std::vector<float> {0.003765,  0.015019,   0.023792,   0.015019,   0.003765});
            return filter;
        break;

        case 3:
            filter.insert(filter.end(), std::vector<float> {0.000036,  0.000363,   0.001446,   0.002291,   0.001446,   0.000363,   0.000036});
            filter.insert(filter.end(), std::vector<float> {0.000363,  0.003676,   0.014662,   0.023226,   0.014662,   0.003676,   0.000363});
            filter.insert(filter.end(), std::vector<float> {0.001446,  0.014662,   0.058488,   0.092651,   0.058488,   0.014662,   0.001446});
            filter.insert(filter.end(), std::vector<float> {0.002291,  0.023226,   0.092651,   0.146768,   0.092651,   0.023226,   0.002291});
            filter.insert(filter.end(), std::vector<float> {0.001446,  0.014662,   0.058488,   0.092651,   0.058488,   0.014662,   0.001446});
            filter.insert(filter.end(), std::vector<float> {0.000363,  0.003676,   0.014662,   0.023226,   0.014662,   0.003676,   0.000363});
            filter.insert(filter.end(), std::vector<float> {0.000036,  0.000363,   0.001446,   0.002291,   0.001446,   0.000363,   0.000036});
            return filter;
        break;

        default:
            std::cerr << "filter() ERROR: can't handle size " << filterSize << std::endl;
            exit (EXIT_FAILURE);
        break;
    }
}

//  Print Filter
void printFilter(std::vector<std::vector<float>> filter)
{
    std::cout << "Filter:" << std::endl;
    for(auto& row:filter)
    {
        for(auto& col:row)
        {
            std::cout << col << ", ";
        }
        std::cout << std::endl;
    }
}
