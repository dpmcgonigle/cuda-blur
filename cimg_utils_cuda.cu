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

namespace cl=cimg_library;
 
/*
*           CUDA FUNCTIONS
*/

/*
*   Split the channels apart.
*   uchar4 is a built-in vector struct with special allignment:
*   https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#vector-types
*
*   c_red, c_green and c_blue are the color channels to be returned from this kernel.
*/
/*  Kept getting segmentation faults from merge_channels
__global__
void split_channels(const uchar4* const image, int rows, int cols, 
                    unsigned char* const c_red,
                    unsigned char* const c_green, 
                    unsigned char* const c_blue)
{
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    //  Set bounds for channel
    if (col >= cols || row >= rows) 
    {
        return;
    }
    int index = row * cols + col;
    c_red[index] = image[index].x;
    c_green[index] = image[index].y;
    c_blue[index] = image[index].z;
}
*/

/*
*   Blur
*/
__global__
void apply_blur_cuda(const unsigned char* const input, unsigned char* const output,
                   int rows, int cols, const float* const filter, const int filterSize)
{
    
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    //  Set bounds for blur filter (no padding)
    if (col >= cols - filterSize || row >= rows - filterSize || col < filterSize || row < filterSize) 
    {
        return;
    }
    int index = row * cols + col;
    /*
    float sum = 0.0;

    for (int frow = row - filterSize; frow <= row + filterSize; frow++)
    {
        for (int fcol = col - filterSize; fcol <= col + filterSize; fcol++)
        {
            //  vector row and column index
            int vrow = frow - row + filterSize;
            int vcol = fcol - col + filterSize;
            sum += filter[vrow*filterSize+vcol] * input[frow*cols+fcol];
        }
    }
    output[index] = sum;
    */

    output[index] = input[index] / 2;
}

/*
*   Bring channels back together
*/
/*  Kept getting segmentation faults on merge_channels
__global__
void merge_channels(const unsigned char* const c_red, 
                    const unsigned char* const c_green, 
                    const unsigned char* const c_blue,
                    uchar4* const output, int rows, int cols)
{
    const int2 thread_2D_pos = make_int2( blockIdx.x * blockDim.x + threadIdx.x, blockIdx.y * blockDim.y + threadIdx.y);
    const int thread_1D_pos = thread_2D_pos.y * cols + thread_2D_pos.x;
    //  Set bounds for channel dims
    if (thread_2D_pos.x >= cols || thread_2D_pos.y >= rows)
    {
        return;
    }

    unsigned char red = c_red[thread_1D_pos];
    unsigned char green = c_green[thread_1D_pos];
    unsigned char blue = c_blue[thread_1D_pos];

    //  Alpha 255 => no transparency
    uchar4 outputPixel = make_uchar4(red, green, blue, 255);

    output[thread_1D_pos] = outputPixel;
}
*/

/*
*       END CUDA KERNELS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

//  Cuda blur
cl::CImg<unsigned char> blur_cuda( cl::CImg<unsigned char> image , int filterSize )
{
    //  Create filter 2D array
    float **filter = new float*[2*filterSize + 1];
    getFilter(filter, filterSize);
    printFilter(filter, filterSize);

    //  Set block size (number of threads per block), then grid size (number of blocks per kernel)
    const dim3 block_size(16,16,1);
    const dim3 grid_size(image.width()/block_size.x+1, image.height()/block_size.y+1,1);

    //  Variables to hold information on cuda memory
    unsigned char *cuda_red, *cuda_green, *cuda_blue;
    float *cuda_filter;
    /*  A NOTE ABOUT *ptr = CImg.data():
    'T *ptr = img.data()' gives you the pointer to the first value of the image 'img'. 
    The overall size of the used memory for one instance image (in bytes) is then 'width*height*depth*dim*sizeof(T)'.
    Now, the ordering of the pixel values in this buffer follows these rules : 
        The values are not interleaved, and are ordered first along the X,Y,Z and V axis respectively 
        (corresponding to the width,height,depth,dim dimensions), 
        starting from the upper-left pixel to the bottom-right pixel of the instane image, with a classical scanline run.
        So, a color image with dim=3 and depth=1, will be stored in memory as :R1R2R3R4R5R6......G1G2G3G4G5G6.......B1B2B3B4B5B6.... 
        (i.e following a 'planar' structure)and not as R1G1B1R2G2B2R3G3B3... (interleaved channels).
    */
    //unsigned char *cuda_image = image.data();
    //unsigned char *cuda_image;

    //  flatten filter
    //cuda_filter = &(filter[0][0]);

    //  Declare GPU memory pointers
    unsigned char *cuda_red_blurred, *cuda_green_blurred, *cuda_blue_blurred;//, *cuda_image_blurred;

    //  Allocate memory to cuda
    int channel_size = image.get_channel(0).size();
    std::cout << "Channel size: " << channel_size << std::endl;
    std::cout << "Unsigned char size: " << sizeof(unsigned char) << std::endl;
    std::cout << "Int size: " << sizeof(int) << std::endl;

    cudaMalloc((void**)&cuda_red, sizeof(unsigned char) * channel_size);
    cudaMalloc((void**)&cuda_red_blurred, sizeof(unsigned char) * channel_size);
    cudaMalloc((void**)&cuda_green, sizeof(unsigned char) * channel_size);
    cudaMalloc((void**)&cuda_green_blurred, sizeof(unsigned char) * channel_size);
    cudaMalloc((void**)&cuda_blue, sizeof(unsigned char) * channel_size);
    cudaMalloc((void**)&cuda_blue_blurred, sizeof(unsigned char) * channel_size);
    cudaMalloc((void**)&cuda_filter, sizeof(float) * (2*filterSize+1) * (2*filterSize+1));
    //cudaMalloc((void**)&cuda_image, sizeof(uchar4) * image.size());
    //cudaMalloc((void**)&cuda_image_blurred, sizeof(uchar4) * image.size());
    //cudaMalloc((void**)&cuda_image, sizeof(unsigned char) * image.size());
    //cudaMalloc((void**)&cuda_image_blurred, sizeof(unsigned char) * image.size());

    //  Transfer image and filter to GPU
    //cudaMemcpy(cuda_image, image.data(), sizeof(uchar4) * image.size(), cudaMemcpyHostToDevice);
    //cudaMemcpy(cuda_image, image.data(), sizeof(unsigned char) * image.size(), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_filter, &(filter[0][0]), sizeof(float) * (2*filterSize+1) * (2*filterSize+1), cudaMemcpyHostToDevice);
    //cudaMemcpy(cuda_red, cuda_red, sizeof(unsigned char) * image.height() * image.width(), cudaMemcpyHostToDevice);
    //cudaMemcpy(cuda_green, cuda_green, sizeof(unsigned char) * image.height() * image.width(), cudaMemcpyHostToDevice);
    //cudaMemcpy(cuda_blue, cuda_blue, sizeof(unsigned char) * image.height() * image.width(), cudaMemcpyHostToDevice);

/*  Kept getting segmentation faults on merge_channels
    //  Split channels
    split_channels<<<grid_size, block_size>>> (reinterpret_cast<uchar4*>(cuda_image),
                                                image.height(), 
                                                image.width(), 
                                                cuda_red, 
                                                cuda_green, 
                                                cuda_blue);
*/

    cudaMemcpy(cuda_red, (unsigned char*)image.get_channel(0), sizeof(unsigned char) * channel_size, cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_green, (unsigned char*)image.get_channel(1), sizeof(unsigned char) * channel_size, cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_blue, (unsigned char*)image.get_channel(2), sizeof(unsigned char) * channel_size, cudaMemcpyHostToDevice);
    
    //  Apply blur
    apply_blur_cuda<<<grid_size, block_size>>> (cuda_red, 
                                                cuda_red_blurred, 
                                                image.height(), 
                                                image.width(), 
                                                cuda_filter, 
                                                filterSize);
    apply_blur_cuda<<<grid_size, block_size>>> (cuda_green, 
                                                cuda_green_blurred, 
                                                image.height(), 
                                                image.width(), 
                                                cuda_filter, 
                                                filterSize);
    apply_blur_cuda<<<grid_size, block_size>>> (cuda_blue, 
                                                cuda_blue_blurred, 
                                                image.height(), 
                                                image.width(), 
                                                cuda_filter, 
                                                filterSize);

    unsigned char *red_blurred =  (unsigned char*)malloc (sizeof(unsigned char) * channel_size);
    unsigned char *green_blurred = (unsigned char*)malloc (sizeof(unsigned char) * channel_size);
    unsigned char *blue_blurred = (unsigned char*)malloc (sizeof(unsigned char) * channel_size);
    cudaMemcpy(&red_blurred, cuda_red_blurred, sizeof(unsigned char) * channel_size, cudaMemcpyDeviceToHost);
    cudaMemcpy(&green_blurred, cuda_green_blurred, sizeof(unsigned char) * channel_size, cudaMemcpyDeviceToHost);
    cudaMemcpy(&blue_blurred, cuda_blue_blurred, sizeof(unsigned char) * channel_size, cudaMemcpyDeviceToHost);
    //cudaMemcpy(&image, cuda_image, image.size(), cudaMemcpyDeviceToHost);


    //  loop through image to get blurred pixels
    for (int row = 0; row < image.height(); row++)
    {
        for (int col = 0; col < image.width(); col++)
        {
            image(col, row, 0, 0) = red_blurred[row*image.width()+col];
            image(col, row, 0, 1) = green_blurred[row*image.width()+col];
            image(col, row, 0, 2) = blue_blurred[row*image.width()+col];
        }
    }


/*  Kept getting segmentation faults on merge_channels
    //  Merge channels
    merge_channels<<<grid_size, block_size>>> (cuda_red_blurred, 
                                                cuda_green_blurred, 
                                                cuda_blue_blurred,
                                                reinterpret_cast<uchar4*>(cuda_image_blurred),
                                                image.height(),
                                                image.width());
*/

    //cudaMemcpy(&image, cuda_image, image.size(), cudaMemcpyDeviceToHost);

    char char_break;
    std::cout << "Pause for character break before freeing up memory: ";
    std::cin >> char_break;

    //  Free up space
    cudaFree(cuda_red);
    cudaFree(cuda_red_blurred);
    cudaFree(cuda_green);
    cudaFree(cuda_green_blurred);
    cudaFree(cuda_blue);
    cudaFree(cuda_blue_blurred);
    //cudaFree(cuda_image);
    //cudaFree(cuda_image_blurred);
    cudaFree(cuda_filter);

    std::cout << "Pause for character break after freeing up memory: ";
    std::cin >> char_break;

    return image;
}


