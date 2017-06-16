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
using namespace cv;

double pixelDistance(double u, double v)
{
    return sqrt(u*u + v*v);
}

double gaussianCoeff(double u, double v, double d0)
{
    double d = pixelDistance(u, v);
    return 1.0 - exp((-d*d) / (2*d0*d0));
    // return exp((-d*d) / (2*d0*d0));
}

Mat createGaussianHighPassFilter(Size size, double cutoffInPixels)
{
    Mat ghpf(size, CV_64FC2);
    
    Point center(size.width / 2, size.height / 2);
    
    for(int u = 0; u < ghpf.rows; u++)
    {
        for(int v = 0; v < ghpf.cols; v++)
        {
            ghpf.at<double>(u, v) = gaussianCoeff(u - center.y, v - center.x, cutoffInPixels);
        }
    }
    
    return ghpf;
}
void shiftDFT(Mat& fImage )
{
    Mat tmp, q0, q1, q2, q3;
    
    // first crop the image, if it has an odd number of rows or columns
    
    fImage = fImage(Rect(0, 0, fImage.cols & -2, fImage.rows & -2));
    
    int cx = fImage.cols/2;
    int cy = fImage.rows/2;
    
    // rearrange the quadrants of Fourier image
    // so that the origin is at the image center
    
    q0 = fImage(Rect(0, 0, cx, cy));
    q1 = fImage(Rect(cx, 0, cx, cy));
    q2 = fImage(Rect(0, cy, cx, cy));
    q3 = fImage(Rect(cx, cy, cx, cy));
    
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}
int main(int argc, const char * argv[])
{
    // Load an image
    Mat inputImage = imread("/Users/AtsuyaSato/Desktop/LPF/lena.jpg", 0);
    imshow("inputImage",inputImage);
    // Convert to float
    Mat fImage;
    inputImage.convertTo(fImage, CV_32F);
    
    // DFT Image
    cout << "Direct transform..." << endl;
    Mat fourierTransform;
    dft(fImage, fourierTransform, DFT_SCALE|DFT_COMPLEX_OUTPUT);
    
    //ここが怪しい。
    Mat highpassfilter = createGaussianHighPassFilter(Size(fourierTransform.rows, fourierTransform.cols), 4.0);
    highpassfilter.convertTo(highpassfilter, 13);
    
    mulSpectrums(fourierTransform, highpassfilter, fourierTransform, DFT_ROWS);
    //multiply(fourierTransform, kernel, fourierTransform);
    
    // IDFT
    cout << "Inverse transform..." << endl;
    Mat inverseTransform;
    dft(fourierTransform, inverseTransform, DFT_INVERSE|DFT_REAL_OUTPUT);
    
    // Back to 8-bits
    Mat finalImage;
    inverseTransform.convertTo(finalImage, CV_8U);
    
    imshow("low-pass filter", finalImage);
    
    //Wait for key
    cvWaitKey();
    
    //Destroy Window
    cvDestroyAllWindows();
    
    return 0;
}
