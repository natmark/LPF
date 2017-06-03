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

int main(int argc, const char * argv[])
{
    //reference : https://blog.unasuke.com/2013/opencv-testprogram-on-xcode/

    //画像を開く
    IplImage* before_img = cvLoadImage("/Users/AtsuyaSato/Desktop/LPF/lena.jpg", CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR );
    if( before_img == NULL )
    {
        return 0;
    }
    
    //ウィンドウの作成
    //cvNamedWindow("test-before", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("test-after" , CV_WINDOW_AUTOSIZE);
    
    //画像変換のためIplImage型で宣言
    IplImage* after_img = cvCreateImage(cvGetSize(before_img), before_img->depth, before_img->nChannels);
    
    //ガウシアンフィルタを使って画像をぼかす
    cvSmooth(before_img, after_img, CV_GAUSSIAN , 9);
    
    //画像をウィンドウに表示
    //cvShowImage("test-before", before_img);
    cvShowImage("test-after", after_img);
    
    //キーの入力待ち
    cvWaitKey();
    
    //すべてのウィンドウを削除
    cvDestroyAllWindows();
    
    //画像データの開放
    cvReleaseImage(&before_img);
    cvReleaseImage(&after_img);
    
    return 0;
}
