//
//  vedio2imgs.cpp
//  LearingOpenCV_Code
//
//  Created by 孙一铀 on 16/7/18.
//  Copyright © 2016年 孙一铀. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>
#define NUM_FRAME 2500 //只处理前300帧，根据视频帧数可修改

void Video_to_image(char* filename)
{
    printf("------------- video to image ... ----------------\n");
    //初始化一个视频文件捕捉器
    CvCapture* capture = cvCaptureFromAVI(filename);
    //获取视频信息
    cvQueryFrame(capture);
    int frameH = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
    int frameW = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
    int fps = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
    int numFrames = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
    printf("\tvideo height : %d\n\tvideo width : %d\n\tfps : %d\n\tframe numbers : %d\n", frameH, frameW, fps, numFrames);
    //定义和初始化变量
    int i = 0;
    IplImage* img = 0;
    char image_name[100];
    
    cvNamedWindow( "mainWin", CV_WINDOW_AUTOSIZE );
    //读取和显示
    while(1)
    {
        
        img = cvQueryFrame(capture); //获取一帧图片
        //if (!img) break;
        
        cvShowImage( "mainWin", img ); //将其显示
        char key = cvWaitKey(2);
        
        sprintf(image_name, "/Users/sunyiyou/Desktop/workspace/数据集/快仓/shit2/%d%s", ++i, ".jpg");//保存的图片名
        
        cvSaveImage( image_name, img); //保存一帧图片
        
        if(i == NUM_FRAME) break;
    }
    cvReleaseCapture(&capture);
    cvDestroyWindow("mainWin");
}
int main(int argc, char *argv[])
{
    char filename[100] = "/Users/sunyiyou/Movies/1.mov";
    Video_to_image(filename); //视频转图片
    return 0;
}
//----------------------------------
