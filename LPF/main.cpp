//
//  main.cpp
//  LPF
//
//  Created by AtsuyaSato on 2017/06/03.
//  Copyright © 2017年 Atsuya Sato. All rights reserved.
//

#include <iostream>
#include <unistd.h>

#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#endif

using namespace std;

// define the kernel
const float Kernel[3][3] = {
    {1/9.0, 1/9.0, 1/9.0},
    {1/9.0, 1/9.0, 1/9.0},
    {1/9.0, 1/9.0, 1/9.0}
};

int main(int argc, const char * argv[])
{
    // https://stackoverflow.com/questions/10269456/inverse-fourier-transformation-in-opencv
    // http://org-technology.com/posts/smoother.html
    
    // Load an image
    cv::Mat inputImage = cv::imread("/Users/AtsuyaSato/Desktop/LPF/lena.jpg", 0);
    
    // Convert to float
    cv::Mat fImage;
    inputImage.convertTo(fImage, CV_32F);
    
    // FFT Image
    std::cout << "Direct transform...\n";
    cv::Mat fourierTransform;
    cv::dft(fImage, fourierTransform, cv::DFT_SCALE|cv::DFT_COMPLEX_OUTPUT);
    
    //filter
    cv::Mat filter = inputImage.clone();
    for(int y = 0; y < inputImage.rows; y++)
        for(int x = 0; x < inputImage.cols; x++)
            filter.at<uchar>(y,x) = 0.0;
    
    for(int y = 0; y < 3; y++)
        for(int x = 0; x < 3; x++)
            filter.at<uchar>(y,x) = Kernel[y][x];
            
    
    // IFFT
    std::cout << "Inverse transform...\n";
    cv::Mat inverseTransform;
    cv::dft(fourierTransform, inverseTransform, cv::DFT_INVERSE|cv::DFT_REAL_OUTPUT);
    
    // Back to 8-bits
    cv::Mat finalImage;
    inverseTransform.convertTo(finalImage, CV_8U);
    
    imshow("low-pass filter", finalImage);
    
    //Wait for key
    cvWaitKey();
    
    //Destroy Window
    cvDestroyAllWindows();
    
    return 0;
}
