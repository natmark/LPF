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
    IplImage* loadImg = cvLoadImage("/Users/AtsuyaSato/Desktop/LPF/lena.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    if(loadImg == NULL) return 0;
    
    cv::Mat mat(loadImg);  //copy constructor

    cv::Mat src = loadImg; //source
    cv::Mat dst = src.clone(); // destination
    
    //initialize pixel value
    for(int y = 0; y < src.rows; y++)
        for(int x = 0; x < src.cols; x++)
            dst.at<uchar>(y,x) = 0.0;
    
    float sum;
    for(int y = 1; y < src.rows - 1; y++){
        for(int x = 1; x < src.cols - 1; x++){
            sum = 0.0;
            for(int k = -1; k <= 1;k++){
                for(int j = -1; j <=1; j++){
                    sum = sum + Kernel[j+1][k+1] * src.at<uchar>(y - j, x - k);
                }
            }
            dst.at<uchar>(y,x) = sum;
        }
    }
    
    //ウィンドウの作成
    cvNamedWindow("lpf" , CV_WINDOW_AUTOSIZE);
    
    IplImage iplImage = dst; // convert type

    //画像をウィンドウに表示
    cvShowImage("lpf", &iplImage);
    
    //キーの入力待ち
    cvWaitKey();
    
    //すべてのウィンドウを削除
    cvDestroyAllWindows();
    
    //画像データの開放
    cvReleaseImage(&loadImg);
    
    return 0;
}
