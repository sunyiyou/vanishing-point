//
//  Drawing.hpp
//  LearingOpenCV_Code
//
//  Created by 孙一铀 on 16/7/21.
//  Copyright © 2016年 孙一铀. All rights reserved.
//

#ifndef Drawing_hpp
#define Drawing_hpp

#include <stdio.h>

#include "opencv.hpp"
using namespace cv;
static void LineAA( Mat& img, Point pt1, Point pt2, const void* color );

static void
Line( Mat& img, Point pt1, Point pt2, const void* _color, int connectivity);
static void
Line2( Mat& img, Point pt1, Point pt2, const void* color );

static void
myFillConvexPoly( Mat& img, const Point* v, int npts, const void* color,
                 int lineType = LINE_8,
                 int shift = 0);

#define ICV_HLINE( ptr, xl, xr, color, pix_size )            \
{                                                            \
uchar* hline_ptr = (uchar*)(ptr) + (xl)*(pix_size);      \
uchar* hline_max_ptr = (uchar*)(ptr) + (xr)*(pix_size);  \
\
for( ; hline_ptr <= hline_max_ptr; hline_ptr += (pix_size))\
{                                                        \
int hline_j;                                         \
for( hline_j = 0; hline_j < (pix_size); hline_j++ )  \
{                                                    \
hline_ptr[hline_j] += 1;   \
}                                                    \
}                                                        \
}
#define FLOAT_ICV_HLINE( ptr, xl, xr, color, pix_size )            \
{                                                            \
float* hline_ptr = (float*)(ptr) + (xl);      \
float* hline_max_ptr = (float*)(ptr) + (xr);  \
\
for( ; hline_ptr <= hline_max_ptr; hline_ptr += 1)\
{                                                        \
int hline_j;                                         \
                                              \
hline_ptr[hline_j] += 1;   \
                                                \
}                                                        \
}
//hline_ptr[hline_j] +=  ((uchar*)color)[hline_j];

/* Correction table depent on the slope */
static const uchar SlopeCorrTable[] = {
    181, 181, 181, 182, 182, 183, 184, 185, 187, 188, 190, 192, 194, 196, 198, 201,
    203, 206, 209, 211, 214, 218, 221, 224, 227, 231, 235, 238, 242, 246, 250, 254
};

/* Gaussian for antialiasing filter */
static const int FilterTable[] = {
    168, 177, 185, 194, 202, 210, 218, 224, 231, 236, 241, 246, 249, 252, 254, 254,
    254, 254, 252, 249, 246, 241, 236, 231, 224, 218, 210, 202, 194, 185, 177, 168,
    158, 149, 140, 131, 122, 114, 105, 97, 89, 82, 75, 68, 62, 56, 50, 45,
    40, 36, 32, 28, 25, 22, 19, 16, 14, 12, 11, 9, 8, 7, 5, 5
};

#endif /* Drawing_hpp */
