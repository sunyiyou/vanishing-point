//
//  Camera.cpp
//  LearingOpenCV_Code
//
//  Created by 孙一铀 on 16/7/18.
//  Copyright © 2016年 孙一铀. All rights reserved.
//

#include <stdio.h>
//
//  main.cpp
//  LearingOpenCV_Code
//
//  Created by 孙一铀 on 16/5/5.
//  Copyright (c) 2016年 孙一铀. All rights reserved.
//
#define imgsrc "/Users/sunyiyou/Pictures/猫头鹰.jpg"
#define imgsrc2 "/Users/sunyiyou/Pictures/仓库.png"
#define ocrdemo3 "/Users/sunyiyou/Desktop/pic/ocrdemo3.JPG"
#define ocrdemo2 "/Users/sunyiyou/Desktop/pic/ocrdemo2.png"


#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include <iostream>
#include <fstream>
#include <sys/time.h>
using namespace std;

long getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
int main2()
{
    ofstream log("/Users/sunyiyou/Movies/kuaicang/1/1.txt");
    
    CvCapture* capture=cvCaptureFromCAM(-1);
    //CvCapture* capture = cvCreateFileCapture("/Users/sunyiyou/Movies/1.mp4");
    CvVideoWriter* video=NULL;
    IplImage* frame=NULL;
    
    
    int n;
    if(!capture) //如果不能打开摄像头给出警告
    {
        cout<<"Can not open the camera."<<endl;
        return -1;
    }
    else
    {
        frame=cvQueryFrame(capture); //首先取得摄像头中的一帧
        
        cvNamedWindow("Camera Video",1); //新建一个窗口
        int i = 0;
        while(i <= 20) // 让它循环200次自动停止录取
        {
            frame=cvQueryFrame(capture); //从CvCapture中获得一帧
            if(!frame)
            {
                cout<<"Can not get frame from the capture."<<endl;
                break;
            }
            stringstream ss;
            ss << "/Users/sunyiyou/Movies/kuaicang/1/" << i << ".jpg";
            
            cvSaveImage(ss.str().c_str(),frame);
            
            log << i << " " << getCurrentTime() << endl;
            
            //n=cvWriteFrame(video,frame); //判断是否写入成功，如果返回的是1，表示写入成功
            cout<<i<<endl;
            cvShowImage("Camera Video",frame); //显示视频内容的图片
            i++;
            if(cvWaitKey(10)>0)
                break; //有其他键盘响应，则退出
        }
        log.close();
        cvReleaseVideoWriter(&video);
        cvReleaseCapture(&capture);
        cvDestroyWindow("Camera Video");
    }
    return 0;
}

int main21()
{
    CvCapture* capture=cvCaptureFromCAM(-1);
    CvVideoWriter* video=NULL;
    IplImage* frame=NULL;
    int n;
    if(!capture) //如果不能打开摄像头给出警告
    {
        cout<<"Can not open the camera."<<endl;
        return -1;
    }
    else
    {
        frame=cvQueryFrame(capture); //首先取得摄像头中的一帧
        video=cvCreateVideoWriter("/Users/sunyiyou/Movies/kuaicang/camera.avi", (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FOURCC), 25,
                                  cvSize(frame->width,frame->height)); //创建CvVideoWriter对象并分配空间
        //保存的文件名为camera.avi，编码要在运行程序时选择，大小就是摄像头视频的大小，帧频率是32
        if(video) //如果能创建CvVideoWriter对象则表明成功
        {
            cout<<"VideoWriter has created."<<endl;
        }
        
        cvNamedWindow("Camera Video",1); //新建一个窗口
        int i = 0;
        while(i <= 20) // 让它循环200次自动停止录取
        {
            frame=cvQueryFrame(capture); //从CvCapture中获得一帧
            if(!frame)
            {
                cout<<"Can not get frame from the capture."<<endl;
                break;
            }
            n=cvWriteFrame(video,frame); //判断是否写入成功，如果返回的是1，表示写入成功
            cout<<n<<endl;
            cvShowImage("Camera Video",frame); //显示视频内容的图片
            i++;
            if(cvWaitKey(2)>0)
                break; //有其他键盘响应，则退出
        }
        
        cvReleaseVideoWriter(&video);
        cvReleaseCapture(&capture);
        cvDestroyWindow("Camera Video");
    }
    return 0;
}
