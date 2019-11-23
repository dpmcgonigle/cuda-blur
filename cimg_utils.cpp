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
#include <cmath>

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
    if (cudaFlag)
    {
        //  Placeholding until cuda function finished
        return blur_sequential(image, filterSize);
    }
    else
    {
        return blur_sequential(image, filterSize);
    }
}

//  Sequential blur
cl::CImg<unsigned char> blur_sequential( cl::CImg<unsigned char> image , int filterSize )
{
    //  Had to ditch vector of vectors, as they apparently can't be sent to cuda
    //std::vector<std::vector<float>> filter = getFilter(filterSize);

    //  Create filter 2D array
    float **filter = new float*[2*filterSize + 1];

    getFilter(filter, filterSize);

    printFilter(filter, filterSize);

    //  Loop over image channels
    cimg_forC(image, c)
    {
        //  Loop rows
        for (int row = filterSize; row < (image.height()-filterSize); row++)
        {
            //  Loop cols
            for (int col = filterSize; col < (image.width()-filterSize); col++)
            {
                float pixelValue=0.0;
                //  Loop filter rows
                for (int frow = row - filterSize; frow <= row + filterSize; frow++)
                {
                    //  Loop filter cols
                    for (int fcol = col - filterSize; fcol <= col + filterSize; fcol++)
                    {
                        //  vector row and column index
                        int vrow = frow - row + filterSize;
                        int vcol = fcol - col + filterSize;
                        pixelValue += ( image(fcol, frow, 0, c) * filter[vrow][vcol] );
                    }
                }

                image(col, row, 0, c) = pixelValue;
            }
        }
    }

    return image;
}

/*      -getFilter-
Create gaussian filter with formula from sources below.
filter is a pointer to a 2d array of floats, 
    the first dimension of which has already been initilized dynamically.
This approach replaced the 2d vector approach, as 2d vectors can't cuda!

Information on gaussian filter from:
Obtained through http://dev.theomader.com/gaussian-kernel-calculator/
https://www.geeksforgeeks.org/gaussian-filter-generation-c/

Example size 1 filter:
0.077847,    0.123317,   0.077847
0.123317,    0.195346,   0.123317
0.077847,    0.123317,   0.077847

Example size 2 filter:
0.003765,  0.015019,   0.023792,   0.015019,   0.003765
0.015019,  0.059912,   0.094907,   0.059912,   0.015019
0.023792,  0.094907,   0.150342,   0.094907,   0.023792
0.015019,  0.059912,   0.094907,   0.059912,   0.015019
0.003765,  0.015019,   0.023792,   0.015019,   0.003765
*/
void getFilter(float **filter, int filterSize)
{
    //  Initialize second dimension of float 2d array
    for (int row=0; row<2*filterSize + 1; row++)
    {
        filter[row] = new float[2*filterSize + 1];
    }

    //  Initilize standard deviation to 1.0
    double sigma = 1.0;
    double r, s = 2.0 * sigma * sigma;

    //  Sum for normalization
    double sum = 0.0;

    //  Generate kernel
    for (int row = -filterSize; row <= filterSize; row++)
    {
        for (int col = -filterSize; col <= filterSize; col++)
        {
            r = sqrt( row * row + col * col );
            filter[row + filterSize][col + filterSize] = (exp(-(r * r) / s)) / (M_PI * s); 
            sum += filter[row + filterSize][col + filterSize];
        }
    }

    //  Normalize kernel
    for (int row = 0; row < 2*filterSize + 1; row++)
    {
        for (int col = 0; col < 2*filterSize + 1; col++)
        {
            filter[row][col] /= sum;
        }
    }
}

//  getFilter   (DEPRECATED)
//  Since you can't put 2d vectors onto cuda, I have abandoned this approach in favor of 2d array
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

        case 4:
            filter.insert(filter.end(), std::vector<float> {0,0.000001,0.000014,0.000055,0.000088,0.000055,0.000014,0.000001,0});
            filter.insert(filter.end(), std::vector<float> {0.000001,0.000036,0.000362,0.001445,0.002289,0.001445,0.000362,0.000036,0.000001});
            filter.insert(filter.end(), std::vector<float> {0.000014,0.000362,0.003672,0.014648,0.023205,0.014648,0.003672,0.000362,0.000014});
            filter.insert(filter.end(), std::vector<float> {0.000055,0.001445,0.014648,0.058434,0.092566,0.058434,0.014648,0.001445,0.000055});
            filter.insert(filter.end(), std::vector<float> {0.000088,0.002289,0.023205,0.092566,0.146634,0.092566,0.023205,0.002289,0.000088});
            filter.insert(filter.end(), std::vector<float> {0.000055,0.001445,0.014648,0.058434,0.092566,0.058434,0.014648,0.001445,0.000055});
            filter.insert(filter.end(), std::vector<float> {0.000014,0.000362,0.003672,0.014648,0.023205,0.014648,0.003672,0.000362,0.000014});
            filter.insert(filter.end(), std::vector<float> {0.000001,0.000036,0.000362,0.001445,0.002289,0.001445,0.000362,0.000036,0.000001});
            filter.insert(filter.end(), std::vector<float> {0,0.000001,0.000014,0.000055,0.000088,0.000055,0.000014,0.000001,0});
            return filter;
        break;
/*
        case 5:
            filter.insert(filter.end(), std::vector<float> {});
            filter.insert(filter.end(), std::vector<float> {});
            filter.insert(filter.end(), std::vector<float> {});
            filter.insert(filter.end(), std::vector<float> {});
            filter.insert(filter.end(), std::vector<float> {});
            filter.insert(filter.end(), std::vector<float> {});
            filter.insert(filter.end(), std::vector<float> {});
            filter.insert(filter.end(), std::vector<float> {});
            filter.insert(filter.end(), std::vector<float> {});
            filter.insert(filter.end(), std::vector<float> {});
            filter.insert(filter.end(), std::vector<float> {});
            return filter;
        break;
*/
        default:
            std::cerr << "filter() ERROR: can't handle size " << filterSize << std::endl;
            exit (EXIT_FAILURE);
        break;
    }
}

//  Print Filter
void printFilter(float **filter, int filterSize)
{
    for (int row=0; row<2*filterSize + 1; row++)
    {
        for (int col=0; col<2*filterSize + 1; col++)
        {
            std::cout << filter[row][col] << ", ";
        }
        std::cout << std::endl;
    }
}

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

