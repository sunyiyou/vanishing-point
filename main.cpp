
#include <vector>
#include <deque>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>
//#include <direct.h>//_mkdir()
#include "opencv.hpp"
#include "GaborFR.hpp"
#include "Drawing.cpp"
using namespace std;

enum gtype{
    EVEN,
    ODD
};
void drawRect(Mat show,Point* ps);
Mat inline minmaxmat(Mat m, double dmin, double dmax);

Point* simpleSamplingPoint(Mat mat, int rows, int cols);
Point* rectAreaSamplingPoint(Mat mat,Rect r, int rows, int cols);
Point* lineSamplingPoint(Mat mat, int rows, int cols, int* realen);
Point* lineAllPoint(int width, int height ,int* realen);

int inline CountVoting(Point* ps,int pslen,float* maxtheta,float resolution, int x, int y);
Point voteResult(Rect roi, Point* ps,int pslen, float* maxtheta, float resolution);
Point quickVoting(Rect roi, Point* ps, int pslen, float* maxtheta, float resolution);
Mat _getGaborKernelNorm( int k,double theta, double lambd, int ktype,int gtype );

//#define showline
//#define showrect

int main(){
//    Mat colormat = imread("/Users/sunyiyou/Desktop/workspace/数据集/快仓/rubbish/70.jpg",IMREAD_COLOR);
//    Mat splits[3];
//    split(colormat, splits);
//    Mat in = splits[0];
    Mat in = imread("/Users/sunyiyou/Desktop/workspace/数据集/快仓/rubbish/470.jpg",0);  // load grayscale
    Mat show;
    in.convertTo(show,CV_8U);
    
    int width = in.size().width;
    int height = in.size().height;
    int ort = 20;
    int ksize = 50;
    const int rows = 50;
    const int cols = 60;
    clock_t start,end;
    //ps 为取样点 作为voter
    Rect roi = Rect(10,height/2,width-10,height/2);
    int plen = 0;
    Point* ps = lineSamplingPoint(in, rows, cols, &plen);
    
    //为voter初始化参数
    float max[plen];
    float maxtheta[plen];
    for (int i = 0; i< plen; i++) {
        max[i] = -MAXFLOAT;
        maxtheta[i] = 0;
    }
    //滤波并选择出最大的方向
    start = clock();
    for(float theta = -CV_PI/2;theta<CV_PI/2;theta+=CV_PI/ort){
        Mat kernel_even = _getGaborKernelNorm(ksize, theta, 16,CV_32F,EVEN);
        Mat kernel_odd = _getGaborKernelNorm(ksize, theta, 16,CV_32F,ODD);
        Mat dst_even,dst_odd,dst;
        cv::filter2D(in, dst_even, CV_32F, kernel_even);
        cv::filter2D(in, dst_odd, CV_32F, kernel_odd);
        
        dst = dst_even.mul(dst_even) + dst_odd.mul(dst_odd);
        
        for (int i = 0; i<plen; i++) {
            if (dst.at<float>(ps[i]) > max[i]) {
                max[i] = dst.at<float>(ps[i]);
                maxtheta[i] = theta*180/CV_PI;
            }
        }
    }
    end = clock();
    printf("Filtering Use Time:%f\n",((double)(end - start)/CLOCKS_PER_SEC));
#ifdef showline
    for (int i = 0; i<plen; i++) {
//        if (abs((maxtheta[i]*CV_PI/180 - CV_PI/2)) >= CV_PI*0.5/ort  &&
//            abs((maxtheta[i]*CV_PI/180 + CV_PI/2)) >= CV_PI*0.5/ort  &&
//            abs(maxtheta[i]*CV_PI/180) >= CV_PI*0.5/ort) {
                line(show,
                     Point(ps[i].x + sin(maxtheta[i]*CV_PI/180) * 10,ps[i].y - cos(maxtheta[i]*CV_PI/180) * 10),
                     Point(ps[i].x - sin(maxtheta[i]*CV_PI/180) * 10,ps[i].y + cos(maxtheta[i]*CV_PI/180) * 10),
                     Scalar(255,255,255));
//        }
    }
#endif
    //准备参数
    int begin_x = width  * 3/8;
    int begin_y = height * 1/3;
    int vwidth = width/4;
    int vheight = height/3;
    float resolution = CV_PI*0.5/ort;
    Rect votroi =  Rect(begin_x,begin_y,vwidth,vheight);
    
    
    //寻找vanishing point
    
    start = clock();
    //Point vanishingpoint = voteResult(votroi, ps, plen, maxtheta, resolution);
    Point vanishingpoint = quickVoting(votroi, ps, plen, maxtheta, resolution);
    end = clock();
    printf("Finding Vanishing Point Use Time:%f\n",((double)(end - start)/CLOCKS_PER_SEC));
    //绘图
#ifdef showrect
    Point prect[4];
    prect[0] = Point(begin_x,begin_y);
    prect[1] = Point(begin_x + vwidth,begin_y);
    prect[2] = Point(begin_x,begin_y + vheight);
    prect[3] = Point(begin_x + vwidth,begin_y + vheight);
    drawRect(show, prect);
#endif
    
    circle(show, vanishingpoint, 5, Scalar(255,255,255));
    
    imshow("img", show);
    waitKey();
    return 0;
}
int main33(){
    Mat a = Mat::zeros(15, 16, CV_32F);
    Mat b = (Mat_<float>(2,3) << 1,2,3,40,55,6);
    
    double d1,d2;int d3[2],d4[2];
    minMaxIdx(b, &d1,&d2,d3,d4);
    cout << d1 << d2 << d4[0] << d4[1]  ;
//    
//    Point prect[3];
//    prect[0] = Point(0,12);
//    prect[1] = Point(6,0);
//    prect[2] = Point(15,4);
//    prect[3] = Point(4,15);
//    
//    uchar color[4];
//    color[0] = 1;
//    color[1] = 63;
//    color[2] = 63;
//    color[3] = 63;
//    
//    Point prect2[3];
//    prect2[0] = Point(2,0);
//    prect2[1] = Point(15,4);
//    prect2[2] = Point(15,8);
//    prect2[3] = Point(0,0);
//    
//    fillConvexPoly(a, prect, 4, Scalar(1,1,1));
//    myFillConvexPoly(a, prect, 4, color);
//    myFillConvexPoly(a, prect2, 4, color);
    //myFillConvexPoly(a, prect, 3, color);
//    bool f = clipLine(Rect(1,1,4,4), prect[1], prect[2]);
//    cout << prect[1] << " " << prect[2] << " " << f;
//    
//    cout << a;
    waitKey();
    return 0;
}



int main3333(){
    Mat in = imread("/Users/sunyiyou/Desktop/workspace/数据集/快仓/rubbish/1.jpg",0);          // load
    Mat src_f;
    in.convertTo(src_f,CV_32F);
    
    int kernel_size = 50;
    Mat kernel_even = _getGaborKernelNorm(kernel_size, 0, 16,CV_32F,EVEN);
    Mat kernel_odd = _getGaborKernelNorm(kernel_size, 0, 16,CV_32F,ODD);
    Mat dst_even,dst_odd,dst;
    cv::filter2D(in, dst_even, CV_32F, kernel_even);
    cv::filter2D(in, dst_odd, CV_32F, kernel_odd);
    
    dst = dst_even.mul(dst_even) + dst_odd.mul(dst_odd);
    
    Mat viz;
    
    dst.convertTo(viz,CV_8U);     // move to proper[0..255] range to show it
    Mat temp = minmaxmat(kernel_odd, 0, 255);
    Mat showkernel;
    temp.convertTo(showkernel,CV_8U);
    
    imshow("k",showkernel);
    
   // imshow("d",viz);
    waitKey();
    return 0;
}


Mat _getGaborKernelNorm( int k,double theta, double lambd, int ktype , int gtype)
{
    double sigma = k / 9;

    int xmin, xmax, ymin, ymax;
    double c = cos(theta), s = sin(theta);
    xmax = k/2;
    ymax = k/2;
    xmin = -xmax;
    ymin = -ymax;
    
    CV_Assert( ktype == CV_32F || ktype == CV_64F );
    CV_Assert( gtype == ODD || gtype == EVEN );
    Mat kernel(ymax - ymin + 1, xmax - xmin + 1, ktype);
    

    double cscale = CV_PI*2/lambd;
    
    for( int y = ymin; y <= ymax; y++ )
        for( int x = xmin; x <= xmax; x++ )
        {
            double a = x*c + y*s;
            double b = -x*s + y*c;
            double v = 0;
            if(gtype == EVEN){
                v = exp(-(4*a*a + b*b)/(8*sigma*sigma))*cos(cscale*a);
            }else if(gtype == ODD){
                v = exp(-(4*a*a + b*b)/(8*sigma*sigma))*sin(cscale*a);
            }
            
            if( ktype == CV_32F )
                kernel.at<float>(ymax - y, xmax - x) = (float)v;
            else
                kernel.at<double>(ymax - y, xmax - x) = v;
        }
    
    Mat C = kernel - mean(kernel);
    Mat ret;
    normalize(C, ret, 1,0, CV_L2);
    return ret;
}

Point* simpleSamplingPoint(Mat mat, int rows, int cols){
    int height = mat.size().height;
    int width = mat.size().width;
    int margin_w = width / (cols + 1);
    int margin_h = height / (rows + 1);
    Point* ps = new Point[rows * cols];
    for (int i = 0; i<rows; i++) {
        for (int j = 0; j<cols; j++) {
            ps[i*cols + j] = Point((j+1)*margin_w,(i+1)*margin_h);
        }
    }
    return ps;
}

Point* rectAreaSamplingPoint(Mat mat,Rect r, int rows, int cols){
    Mat roi = mat(Range(r.y,r.y+r.height),Range(r.x,r.x+r.width));
    
    int height = roi.size().height;
    int width = roi.size().width;
    int margin_w = width / (cols + 1);
    int margin_h = height / (rows + 1);
    Point* ps = new Point[rows * cols];
    for (int i = 0; i<rows; i++) {
        for (int j = 0; j<cols; j++) {
            ps[i*cols + j] = Point((j+1)*margin_w+r.x,(i+1)*margin_h+r.y);
        }
    }
    return ps;
}
Point* lineAllPoint(int width, int height ,int* realen){
    const int maxlen = height*width;
    Point* psret = new Point[maxlen];
    Point center = Point(width/2,height/2);
    Point l1l = Point(0,height*3/4);
    Point l1r = Point(width*1/4,height);
    Point l2l = Point(width*3/4,height);
    Point l2r = Point(width,height*3/4);
    float theta1l = atan((float)(l1l.y - center.y)/(float)(l1l.x - center.x));
    float theta1r = atan((float)(l1r.y - center.y)/(float)(l1r.x - center.x));
    float theta2l = atan((float)(l2l.y - center.y)/(float)(l2l.x - center.x));
    float theta2r = atan((float)(l2r.y - center.y)/(float)(l2r.x - center.x));
    float theta;
    *realen = 0;
    for (int y = height/2; y<height; y++) {
        for (int x = 0; x<width; x++) {
            theta = atan((float)(y - center.y)/(float)(x - center.x));
            if (((theta < theta1l && theta > theta1r)||(theta > theta2r && theta < theta2l))){
                psret[(*realen)++] = Point(x,y);
            }
        }
    }
    return psret;
}
Point* lineSamplingPoint(Mat mat, int rows, int cols ,int* realen){
    int height = mat.size().height;
    int width = mat.size().width;
    Point center = Point(width/2,height/2);
    Point l1l = Point(0,height*3/4);
    Point l1r = Point(width*1/3,height);
    Point l2l = Point(width*2/3,height);
    Point l2r = Point(width,height*3/4);
    float theta1l = atan((float)(l1l.y - center.y)/(float)(l1l.x - center.x));
    float theta1r = atan((float)(l1r.y - center.y)/(float)(l1r.x - center.x));
    float theta2l = atan((float)(l2l.y - center.y)/(float)(l2l.x - center.x));
    float theta2r = atan((float)(l2r.y - center.y)/(float)(l2r.x - center.x));
    
    Rect roi = Rect(10,height/2,width-20,height/2);
    Point* ps = rectAreaSamplingPoint(mat, roi, rows, cols);
    const int maxlen = rows * cols;
    Point* psret = new Point[maxlen];
    *realen = 0;
    float theta;
    for (int i = 0; i<rows * cols; i++) {
        theta = atan((float)(ps[i].y - center.y)/(float)(ps[i].x - center.x));
        if (((theta < theta1l && theta > theta1r)||(theta > theta2r && theta < theta2l))){
            psret[(*realen)++] = ps[i];
        }
    }
    return psret;
}

int inline CountVoting(Point* ps,int pslen,float* maxtheta,float resolution, int x, int y){
    int count = 0;
    for (int i = 0; i<pslen; i++) {
        float bias = maxtheta[i]*CV_PI/180 - atan((float)(y-ps[i].y)/(float)(x-ps[i].x));
        if(abs((maxtheta[i]*CV_PI/180 - CV_PI/2)) >= resolution  &&
           abs((maxtheta[i]*CV_PI/180 + CV_PI/2)) >= resolution &&
           abs(maxtheta[i]*CV_PI/180) >= resolution){
            if(abs(bias + CV_PI/2) < resolution ||  abs(bias - CV_PI/2) < resolution) count++;
        }
    }
    return count;
}
Point voteResult(Rect roi, Point* ps,int pslen, float* maxtheta, float resolution){
    int begin_x = roi.x;
    int begin_y = roi.y;

    Mat voting = Mat::zeros(roi.height, roi.width, CV_32F);
    int max_votes = -1;
    Point maxp;
    for (int y = 0; y < roi.height; y++) {
        for (int x = 0; x < roi.width; x++) {
            int votes = CountVoting(ps, pslen, maxtheta, resolution, x+begin_x, y+begin_y);
            if (votes > max_votes) {
                max_votes = votes;
                maxp = Point(x,y);
            }
            voting.at<float>(y,x) = votes;
        }
    }
    
    Mat temp = minmaxmat(voting, 0, 255);
    Mat showVoting;
    temp.convertTo(showVoting, CV_8U);
    
    circle(showVoting, maxp, 5, Scalar(0,0,0));
    imshow("vote",showVoting);
    
    return Point(maxp.x+begin_x,maxp.y+begin_y);
}

Point quickVoting(Rect roi, Point* ps, int pslen, float* maxtheta, float resolution){
    #define LARGE_BIAS 1000
    Mat vote = Mat::zeros(roi.height, roi.width, CV_32F);
//    Scalar color = Scalar(1,1,1);
    uchar color[4];
    color[0] = 63;
    color[1] = 63;
    color[2] = 63;
    color[3] = 63;
    
    for (int i = 0; i<pslen; i++) {
       // if(i == 11){
            ;
        //}
        if (!(abs((maxtheta[i]*CV_PI/180 - CV_PI/2)) >= resolution  &&
            abs((maxtheta[i]*CV_PI/180 + CV_PI/2)) >= resolution  &&
            abs(maxtheta[i]*CV_PI/180) >= resolution))
            continue;
        float theta1 = (maxtheta[i])*CV_PI/180 + resolution;
        float theta2 = (maxtheta[i])*CV_PI/180 - resolution;
        Point boundary[4];
        boundary[0] = Point(ps[i].x + LARGE_BIAS,ps[i].y - LARGE_BIAS/tan(theta1));
        boundary[1] = Point(ps[i].x - LARGE_BIAS,ps[i].y + LARGE_BIAS/tan(theta1));
        boundary[2] = Point(ps[i].x + LARGE_BIAS,ps[i].y - LARGE_BIAS/tan(theta2));
        boundary[3] = Point(ps[i].x - LARGE_BIAS,ps[i].y + LARGE_BIAS/tan(theta2));
        
        bool f1 = clipLine(roi, boundary[0], boundary[1]);
        bool f2 = clipLine(roi, boundary[2], boundary[3]);
        
        float tempx,tempy;
        boundary[0].x = boundary[0].x - roi.x;
        boundary[0].y = boundary[0].y - roi.y;
        boundary[1].x = boundary[1].x - roi.x;
        boundary[1].y = boundary[1].y - roi.y;
        tempx = boundary[2].x - roi.x;
        tempy = boundary[2].y - roi.y;
        boundary[2].x = boundary[3].x - roi.x;
        boundary[2].y = boundary[3].y - roi.y;
        boundary[3].x = tempx;
        boundary[3].y = tempy;
        
        if (f1 + f2 == 2) {
            myFillConvexPoly(vote, boundary, 4, color);
        }
        
    }
    int maxIdx[2];
    minMaxIdx(vote, NULL,NULL,NULL,maxIdx);
    Mat temp = minmaxmat(vote, 0, 255);
    Mat showVoting;
    
    
    temp.convertTo(showVoting, CV_8U);
    imshow("vote2",showVoting);
    return Point(maxIdx[0]+roi.x,maxIdx[1]+roi.y);
}
void drawRect(Mat show,Point* ps){
    line(show,ps[0],ps[1],Scalar(255,255,255));
    line(show,ps[1],ps[3],Scalar(255,255,255));
    line(show,ps[3],ps[2],Scalar(255,255,255));
    line(show,ps[2],ps[0],Scalar(255,255,255));
}
Mat inline minmaxmat(Mat m, double dmin, double dmax){
    double min;
    double max;
    minMaxIdx(m, &min, &max);
    
    return  (m - min) * (dmax - dmin)  / (max- min);
    
}
