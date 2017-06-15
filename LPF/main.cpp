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

double pixelDistance(double u, double v)
{
    return cv::sqrt(u*u + v*v);
}

double gaussianCoeff(double u, double v, double d0)
{
    double d = pixelDistance(u, v);
    return 1.0 - cv::exp((-d*d) / (2*d0*d0));
    // return cv::exp((-d*d) / (2*d0*d0));
}

cv::Mat createGaussianHighPassFilter(cv::Size size, double cutoffInPixels)
{
    cv::Mat ghpf(size, CV_64FC2);
    
    cv::Point center(size.width / 2, size.height / 2);
    
    for(int u = 0; u < ghpf.rows; u++)
    {
        for(int v = 0; v < ghpf.cols; v++)
        {
            ghpf.at<double>(u, v) = gaussianCoeff(u - center.y, v - center.x, cutoffInPixels);
        }
    }
    
    return ghpf;
}
void shiftDFT(cv::Mat& fImage )
{
    cv::Mat tmp, q0, q1, q2, q3;
    
    // first crop the image, if it has an odd number of rows or columns
    
    fImage = fImage(cv::Rect(0, 0, fImage.cols & -2, fImage.rows & -2));
    
    int cx = fImage.cols/2;
    int cy = fImage.rows/2;
    
    // rearrange the quadrants of Fourier image
    // so that the origin is at the image center
    
    q0 = fImage(cv::Rect(0, 0, cx, cy));
    q1 = fImage(cv::Rect(cx, 0, cx, cy));
    q2 = fImage(cv::Rect(0, cy, cx, cy));
    q3 = fImage(cv::Rect(cx, cy, cx, cy));
    
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}
int main(int argc, const char * argv[])
{
    // https://stackoverflow.com/questions/10269456/inverse-fourier-transformation-in-opencv
    // http://org-technology.com/posts/smoother.html
    
    // Load an image
    cv::Mat inputImage = cv::imread("/Users/AtsuyaSato/Desktop/LPF/lena.jpg", 0);
    imshow("inputImage",inputImage);
    // Convert to float
    cv::Mat fImage;
    inputImage.convertTo(fImage, CV_32F);
    
    // DFT Image
    std::cout << "Direct transform...\n";
    cv::Mat fourierTransform;
    cv::dft(fImage, fourierTransform, cv::DFT_SCALE|cv::DFT_COMPLEX_OUTPUT);
    
    //ここが怪しい。
    cv::Mat highpassfilter = createGaussianHighPassFilter(cv::Size(fourierTransform.rows, fourierTransform.cols), 16.0);
    highpassfilter.convertTo(highpassfilter, 13);
    
    shiftDFT(fourierTransform);
    cv::multiply(fourierTransform, highpassfilter, fourierTransform);
    
    // IDFT
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
