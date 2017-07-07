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
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif

using namespace std;
using namespace cv;

Mat shiftquads(Mat &magI,int cx,int cy){
    Mat q0(magI, Rect(0, 0, cx, cy));
    Mat q1(magI, Rect(cx, 0, cx, cy));
    Mat q2(magI, Rect(0, cy, cx, cy));
    Mat q3(magI, Rect(cx, cy, cx, cy));

    Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
    return magI;
}

Mat createGaussianLowPassFilter(Mat complex){
    Mat ghpf( complex.size(), complex.type(), CV_32F);

    int midx = ghpf.rows / 2;
    int midy = ghpf.cols / 2;

    int d0 = 20; //threshold
    
    for(int u = 0; u < ghpf.rows; u++)
    {
        for(int v = 0; v < ghpf.cols; v++)
        {
            if( (sqrt(pow(u-midx,2)+pow(v-midy,2))>d0) )
                ghpf.at<double>(u,v)=0.0;
        }
    }

    shiftquads(ghpf,midx,midy);
    return ghpf;
}

int main(int argc, char** argv){
    if(argc < 2) return -1;
    
    // Load an image
    Mat inputImage = imread(argv[1], 0);
    if(inputImage.empty()) return -1;
    
    imshow("inputImage",inputImage);
    
    //expand input image to optimal size
    Mat padded;
    int m = getOptimalDFTSize( inputImage.rows );
    int n = getOptimalDFTSize( inputImage.cols );
    
    copyMakeBorder(inputImage, padded, 0, m - inputImage.rows, 0, n - inputImage.cols, BORDER_CONSTANT, Scalar::all(0));
    
    Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
    Mat complexI;
    merge(planes, 2, complexI);
    
    // DFT
    Mat fourierTransform;
    dft(complexI, fourierTransform);
    
    // lowPassFilter
    Mat lowPassFilter = createGaussianLowPassFilter(fourierTransform);
    cv::mulSpectrums(fourierTransform,lowPassFilter,fourierTransform,DFT_COMPLEX_OUTPUT);
    
    // IDFT
    Mat inverseTransform;
    dft(fourierTransform, inverseTransform, DFT_INVERSE|DFT_REAL_OUTPUT);
    
    normalize(inverseTransform, inverseTransform, 0, 1, CV_MINMAX);

    imshow("Filtered Image", inverseTransform);
    
    // Wait for key
    waitKey();
    
    // Destroy Window
    cvDestroyAllWindows();
    
    return 0;
}
