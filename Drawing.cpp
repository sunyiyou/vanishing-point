//
//  Drawing.cpp
//  LearingOpenCV_Code
//
//  Created by 孙一铀 on 16/7/21.
//  Copyright © 2016年 孙一铀. All rights reserved.
//

#include "Drawing.hpp"



enum { XY_SHIFT = 16, XY_ONE = 1 << XY_SHIFT, DRAWING_STORAGE_BLOCK = (1<<12) - 256 };
static void myFillConvexPoly( Mat& img, const Point* v, int npts, const void* color, int line_type, int shift )
{
    struct
    {
        int idx, di;
        int x, dx, ye;
    }
    edge[2];
    
    int delta = shift ? 1 << (shift - 1) : 0;
    int i, y, imin = 0, left = 0, right = 1, x1, x2;
    int edges = npts;
    int xmin, xmax, ymin, ymax;
    uchar* ptr = img.ptr();
    Size size = img.size();
    int pix_size = (int)img.elemSize();
    Point p0;
    int delta1, delta2;
    
    if( line_type < CV_AA )
        delta1 = delta2 = XY_ONE >> 1;
    else
        delta1 = XY_ONE - 1, delta2 = 0;
    
    p0 = v[npts - 1];
    p0.x <<= XY_SHIFT - shift;
    p0.y <<= XY_SHIFT - shift;
    
    assert( 0 <= shift && shift <= XY_SHIFT );
    xmin = xmax = v[0].x;
    ymin = ymax = v[0].y;
    
    for( i = 0; i < npts; i++ )
    {
        Point p = v[i];
        if( p.y < ymin )
        {
            ymin = p.y;
            imin = i;
        }
        
        ymax = std::max( ymax, p.y );
        xmax = std::max( xmax, p.x );
        xmin = MIN( xmin, p.x );
        
        p.x <<= XY_SHIFT - shift;
        p.y <<= XY_SHIFT - shift;
        
//        if( line_type <= 8 )
//        {
//            if( shift == 0 )
//            {
//                Point pt0, pt1;
//                pt0.x = p0.x >> XY_SHIFT;
//                pt0.y = p0.y >> XY_SHIFT;
//                pt1.x = p.x >> XY_SHIFT;
//                pt1.y = p.y >> XY_SHIFT;
//                Line( img, pt0, pt1, color, line_type );
//            }
//            else
//                Line2( img, p0, p, color );
//        }
//        else
//            LineAA( img, p0, p, color );
        p0 = p;
    }
    
    xmin = (xmin + delta) >> shift;
    xmax = (xmax + delta) >> shift;
    ymin = (ymin + delta) >> shift;
    ymax = (ymax + delta) >> shift;
    
    if( npts < 3 || xmax < 0 || ymax < 0 || xmin >= size.width || ymin >= size.height )
        return;
    
    ymax = MIN( ymax, size.height - 1 );
    edge[0].idx = edge[1].idx = imin;
    
    edge[0].ye = edge[1].ye = y = ymin;
    edge[0].di = 1;
    edge[1].di = npts - 1;
    
    ptr += img.step*y;
    
    do
    {
        if( line_type < CV_AA || y < ymax || y == ymin )
        {
            for( i = 0; i < 2; i++ )
            {
                if( y >= edge[i].ye )
                {
                    int idx = edge[i].idx, di = edge[i].di;
                    int xs = 0, xe, ye, ty = 0;
                    
                    for(;;)
                    {
                        ty = (v[idx].y + delta) >> shift;
                        if( ty > y || edges == 0 )
                            break;
                        xs = v[idx].x;
                        idx += di;
                        idx -= ((idx < npts) - 1) & npts;   /* idx -= idx >= npts ? npts : 0 */
                        edges--;
                    }
                    
                    ye = ty;
                    xs <<= XY_SHIFT - shift;
                    xe = v[idx].x << (XY_SHIFT - shift);
                    
                    /* no more edges */
                    if( y >= ye )
                        return;
                    
                    edge[i].ye = ye;
                    edge[i].dx = ((xe - xs)*2 + (ye - y)) / (2 * (ye - y));
                    edge[i].x = xs;
                    edge[i].idx = idx;
                }
            }
        }
        
        if( edge[left].x > edge[right].x )
        {
            left ^= 1;
            right ^= 1;
        }
        
        x1 = edge[left].x;
        x2 = edge[right].x;
        
        if( y >= 0 )
        {
            int xx1 = (x1 + delta1) >> XY_SHIFT;
            int xx2 = (x2 + delta2) >> XY_SHIFT;
            
            if( xx2 >= 0 && xx1 < size.width )
            {
                if( xx1 < 0 )
                    xx1 = 0;
                if( xx2 >= size.width )
                    xx2 = size.width - 1;
                //ICV_HLINE( ptr, xx1, xx2, color, pix_size );
                FLOAT_ICV_HLINE( ptr, xx1, xx2, color, pix_size );
            }
        }
        
        x1 += edge[left].dx;
        x2 += edge[right].dx;
        
        edge[left].x = x1;
        edge[right].x = x2;
        ptr += img.step;
    }
    while( ++y <= ymax );
}




static void
Line( Mat& img, Point pt1, Point pt2,
     const void* _color, int connectivity = 8 )
{
    if( connectivity == 0 )
        connectivity = 8;
    if( connectivity == 1 )
        connectivity = 4;
    
    LineIterator iterator(img, pt1, pt2, connectivity, true);
    int i, count = iterator.count;
    int pix_size = (int)img.elemSize();
    const uchar* color = (const uchar*)_color;
    
    for( i = 0; i < count; i++, ++iterator )
    {
        uchar* ptr = *iterator;
        if( pix_size == 1 ){
            //ptr[0] = color[0];
        }else if( pix_size == 3 )
        {
//            ptr[0] = color[0];
//            ptr[1] = color[1];
//            ptr[2] = color[2];
        }
        else{
            //memcpy( *iterator, color, pix_size );
        }
    }
}

static void
LineAA( Mat& img, Point pt1, Point pt2, const void* color )
{
    int dx, dy;
    int ecount, scount = 0;
    int slope;
    int ax, ay;
    int x_step, y_step;
    int i, j;
    int ep_table[9];
    int cb = ((uchar*)color)[0], cg = ((uchar*)color)[1], cr = ((uchar*)color)[2];
    int _cb, _cg, _cr;
    int nch = img.channels();
    uchar* ptr = img.data;
    size_t step = img.step;
    Size size = img.size();
    
    if( !((nch == 1 || nch == 3) && img.depth() == CV_8U) )
    {
        Line(img, pt1, pt2, color);
        return;
    }
    
    pt1.x -= XY_ONE*2;
    pt1.y -= XY_ONE*2;
    pt2.x -= XY_ONE*2;
    pt2.y -= XY_ONE*2;
    ptr += img.step*2 + 2*nch;
    
    size.width = ((size.width - 5) << XY_SHIFT) + 1;
    size.height = ((size.height - 5) << XY_SHIFT) + 1;
    
    if( !clipLine( size, pt1, pt2 ))
        return;
    
    dx = pt2.x - pt1.x;
    dy = pt2.y - pt1.y;
    
    j = dx < 0 ? -1 : 0;
    ax = (dx ^ j) - j;
    i = dy < 0 ? -1 : 0;
    ay = (dy ^ i) - i;
    
    if( ax > ay )
    {
        dx = ax;
        dy = (dy ^ j) - j;
        pt1.x ^= pt2.x & j;
        pt2.x ^= pt1.x & j;
        pt1.x ^= pt2.x & j;
        pt1.y ^= pt2.y & j;
        pt2.y ^= pt1.y & j;
        pt1.y ^= pt2.y & j;
        
        x_step = XY_ONE;
        y_step = (int) (((int64) dy << XY_SHIFT) / (ax | 1));
        pt2.x += XY_ONE;
        ecount = (pt2.x >> XY_SHIFT) - (pt1.x >> XY_SHIFT);
        j = -(pt1.x & (XY_ONE - 1));
        pt1.y += (int) ((((int64) y_step) * j) >> XY_SHIFT) + (XY_ONE >> 1);
        slope = (y_step >> (XY_SHIFT - 5)) & 0x3f;
        slope ^= (y_step < 0 ? 0x3f : 0);
        
        /* Get 4-bit fractions for end-point adjustments */
        i = (pt1.x >> (XY_SHIFT - 7)) & 0x78;
        j = (pt2.x >> (XY_SHIFT - 7)) & 0x78;
    }
    else
    {
        dy = ay;
        dx = (dx ^ i) - i;
        pt1.x ^= pt2.x & i;
        pt2.x ^= pt1.x & i;
        pt1.x ^= pt2.x & i;
        pt1.y ^= pt2.y & i;
        pt2.y ^= pt1.y & i;
        pt1.y ^= pt2.y & i;
        
        x_step = (int) (((int64) dx << XY_SHIFT) / (ay | 1));
        y_step = XY_ONE;
        pt2.y += XY_ONE;
        ecount = (pt2.y >> XY_SHIFT) - (pt1.y >> XY_SHIFT);
        j = -(pt1.y & (XY_ONE - 1));
        pt1.x += (int) ((((int64) x_step) * j) >> XY_SHIFT) + (XY_ONE >> 1);
        slope = (x_step >> (XY_SHIFT - 5)) & 0x3f;
        slope ^= (x_step < 0 ? 0x3f : 0);
        
        /* Get 4-bit fractions for end-point adjustments */
        i = (pt1.y >> (XY_SHIFT - 7)) & 0x78;
        j = (pt2.y >> (XY_SHIFT - 7)) & 0x78;
    }
    
    slope = (slope & 0x20) ? 0x100 : SlopeCorrTable[slope];
    
    /* Calc end point correction table */
    {
        int t0 = slope << 7;
        int t1 = ((0x78 - i) | 4) * slope;
        int t2 = (j | 4) * slope;
        
        ep_table[0] = 0;
        ep_table[8] = slope;
        ep_table[1] = ep_table[3] = ((((j - i) & 0x78) | 4) * slope >> 8) & 0x1ff;
        ep_table[2] = (t1 >> 8) & 0x1ff;
        ep_table[4] = ((((j - i) + 0x80) | 4) * slope >> 8) & 0x1ff;
        ep_table[5] = ((t1 + t0) >> 8) & 0x1ff;
        ep_table[6] = (t2 >> 8) & 0x1ff;
        ep_table[7] = ((t2 + t0) >> 8) & 0x1ff;
    }
    
    if( nch == 3 )
    {
#define  ICV_PUT_POINT()            \
{                                   \
_cb = tptr[0];                  \
_cb += ((cb - _cb)*a + 127)>> 8;\
_cg = tptr[1];                  \
_cg += ((cg - _cg)*a + 127)>> 8;\
_cr = tptr[2];                  \
_cr += ((cr - _cr)*a + 127)>> 8;\
tptr[0] = (uchar)_cb;           \
tptr[1] = (uchar)_cg;           \
tptr[2] = (uchar)_cr;           \
}
        if( ax > ay )
        {
            ptr += (pt1.x >> XY_SHIFT) * 3;
            
            while( ecount >= 0 )
            {
                uchar *tptr = ptr + ((pt1.y >> XY_SHIFT) - 1) * step;
                
                int ep_corr = ep_table[(((scount >= 2) + 1) & (scount | 2)) * 3 +
                                       (((ecount >= 2) + 1) & (ecount | 2))];
                int a, dist = (pt1.y >> (XY_SHIFT - 5)) & 31;
                
                a = (ep_corr * FilterTable[dist + 32] >> 8) & 0xff;
                ICV_PUT_POINT();
                ICV_PUT_POINT();
                
                tptr += step;
                a = (ep_corr * FilterTable[dist] >> 8) & 0xff;
                ICV_PUT_POINT();
                ICV_PUT_POINT();
                
                tptr += step;
                a = (ep_corr * FilterTable[63 - dist] >> 8) & 0xff;
                ICV_PUT_POINT();
                ICV_PUT_POINT();
                
                pt1.y += y_step;
                ptr += 3;
                scount++;
                ecount--;
            }
        }
        else
        {
            ptr += (pt1.y >> XY_SHIFT) * step;
            
            while( ecount >= 0 )
            {
                uchar *tptr = ptr + ((pt1.x >> XY_SHIFT) - 1) * 3;
                
                int ep_corr = ep_table[(((scount >= 2) + 1) & (scount | 2)) * 3 +
                                       (((ecount >= 2) + 1) & (ecount | 2))];
                int a, dist = (pt1.x >> (XY_SHIFT - 5)) & 31;
                
                a = (ep_corr * FilterTable[dist + 32] >> 8) & 0xff;
                ICV_PUT_POINT();
                ICV_PUT_POINT();
                
                tptr += 3;
                a = (ep_corr * FilterTable[dist] >> 8) & 0xff;
                ICV_PUT_POINT();
                ICV_PUT_POINT();
                
                tptr += 3;
                a = (ep_corr * FilterTable[63 - dist] >> 8) & 0xff;
                ICV_PUT_POINT();
                ICV_PUT_POINT();
                
                pt1.x += x_step;
                ptr += step;
                scount++;
                ecount--;
            }
        }
#undef ICV_PUT_POINT
    }
    else
    {
#define  ICV_PUT_POINT()            \
{                                   \
_cb = tptr[0];                  \
_cb += ((cb - _cb)*a + 127)>> 8;\
tptr[0] = (uchar)_cb;           \
}
        
        if( ax > ay )
        {
            ptr += (pt1.x >> XY_SHIFT);
            
            while( ecount >= 0 )
            {
                uchar *tptr = ptr + ((pt1.y >> XY_SHIFT) - 1) * step;
                
                int ep_corr = ep_table[(((scount >= 2) + 1) & (scount | 2)) * 3 +
                                       (((ecount >= 2) + 1) & (ecount | 2))];
                int a, dist = (pt1.y >> (XY_SHIFT - 5)) & 31;
                
                a = (ep_corr * FilterTable[dist + 32] >> 8) & 0xff;
                ICV_PUT_POINT();
                ICV_PUT_POINT();
                
                tptr += step;
                a = (ep_corr * FilterTable[dist] >> 8) & 0xff;
                ICV_PUT_POINT();
                ICV_PUT_POINT();
                
                tptr += step;
                a = (ep_corr * FilterTable[63 - dist] >> 8) & 0xff;
                ICV_PUT_POINT();
                ICV_PUT_POINT();
                
                pt1.y += y_step;
                ptr++;
                scount++;
                ecount--;
            }
        }
        else
        {
            ptr += (pt1.y >> XY_SHIFT) * step;
            
            while( ecount >= 0 )
            {
                uchar *tptr = ptr + ((pt1.x >> XY_SHIFT) - 1);
                
                int ep_corr = ep_table[(((scount >= 2) + 1) & (scount | 2)) * 3 +
                                       (((ecount >= 2) + 1) & (ecount | 2))];
                int a, dist = (pt1.x >> (XY_SHIFT - 5)) & 31;
                
                a = (ep_corr * FilterTable[dist + 32] >> 8) & 0xff;
                ICV_PUT_POINT();
                ICV_PUT_POINT();
                
                tptr++;
                a = (ep_corr * FilterTable[dist] >> 8) & 0xff;
                ICV_PUT_POINT();
                ICV_PUT_POINT();
                
                tptr++;
                a = (ep_corr * FilterTable[63 - dist] >> 8) & 0xff;
                ICV_PUT_POINT();
                ICV_PUT_POINT();
                
                pt1.x += x_step;
                ptr += step;
                scount++;
                ecount--;
            }
        }
#undef ICV_PUT_POINT
    }
}


static void
Line2( Mat& img, Point pt1, Point pt2, const void* color )
{
    int dx, dy;
    int ecount;
    int ax, ay;
    int i, j, x, y;
    int x_step, y_step;
    int cb = ((uchar*)color)[0];
    int cg = ((uchar*)color)[1];
    int cr = ((uchar*)color)[2];
    int pix_size = (int)img.elemSize();
    uchar *ptr = img.data, *tptr;
    size_t step = img.step;
    Size size = img.size(), sizeScaled(size.width*XY_ONE, size.height*XY_ONE);
    
    //assert( img && (nch == 1 || nch == 3) && img.depth() == CV_8U );
    
    if( !clipLine( sizeScaled, pt1, pt2 ))
        return;
    
    dx = pt2.x - pt1.x;
    dy = pt2.y - pt1.y;
    
    j = dx < 0 ? -1 : 0;
    ax = (dx ^ j) - j;
    i = dy < 0 ? -1 : 0;
    ay = (dy ^ i) - i;
    
    if( ax > ay )
    {
        dx = ax;
        dy = (dy ^ j) - j;
        pt1.x ^= pt2.x & j;
        pt2.x ^= pt1.x & j;
        pt1.x ^= pt2.x & j;
        pt1.y ^= pt2.y & j;
        pt2.y ^= pt1.y & j;
        pt1.y ^= pt2.y & j;
        
        x_step = XY_ONE;
        y_step = (int) (((int64) dy << XY_SHIFT) / (ax | 1));
        ecount = (pt2.x - pt1.x) >> XY_SHIFT;
    }
    else
    {
        dy = ay;
        dx = (dx ^ i) - i;
        pt1.x ^= pt2.x & i;
        pt2.x ^= pt1.x & i;
        pt1.x ^= pt2.x & i;
        pt1.y ^= pt2.y & i;
        pt2.y ^= pt1.y & i;
        pt1.y ^= pt2.y & i;
        
        x_step = (int) (((int64) dx << XY_SHIFT) / (ay | 1));
        y_step = XY_ONE;
        ecount = (pt2.y - pt1.y) >> XY_SHIFT;
    }
    
    pt1.x += (XY_ONE >> 1);
    pt1.y += (XY_ONE >> 1);
    
    if( pix_size == 3 )
    {
#define  ICV_PUT_POINT(_x,_y)   \
x = (_x); y = (_y);             \
if( 0 <= x && x < size.width && \
0 <= y && y < size.height ) \
{                               \
tptr = ptr + y*step + x*3;  \
tptr[0] = (uchar)cb;        \
tptr[1] = (uchar)cg;        \
tptr[2] = (uchar)cr;        \
}
        
        ICV_PUT_POINT((pt2.x + (XY_ONE >> 1)) >> XY_SHIFT,
                      (pt2.y + (XY_ONE >> 1)) >> XY_SHIFT);
        
        if( ax > ay )
        {
            pt1.x >>= XY_SHIFT;
            
            while( ecount >= 0 )
            {
                ICV_PUT_POINT(pt1.x, pt1.y >> XY_SHIFT);
                pt1.x++;
                pt1.y += y_step;
                ecount--;
            }
        }
        else
        {
            pt1.y >>= XY_SHIFT;
            
            while( ecount >= 0 )
            {
                ICV_PUT_POINT(pt1.x >> XY_SHIFT, pt1.y);
                pt1.x += x_step;
                pt1.y++;
                ecount--;
            }
        }
        
#undef ICV_PUT_POINT
    }
    else if( pix_size == 1 )
    {
#define  ICV_PUT_POINT(_x,_y) \
x = (_x); y = (_y);           \
if( 0 <= x && x < size.width && \
0 <= y && y < size.height ) \
{                           \
tptr = ptr + y*step + x;\
tptr[0] = (uchar)cb;    \
}
        
        ICV_PUT_POINT((pt2.x + (XY_ONE >> 1)) >> XY_SHIFT,
                      (pt2.y + (XY_ONE >> 1)) >> XY_SHIFT);
        
        if( ax > ay )
        {
            pt1.x >>= XY_SHIFT;
            
            while( ecount >= 0 )
            {
                ICV_PUT_POINT(pt1.x, pt1.y >> XY_SHIFT);
                pt1.x++;
                pt1.y += y_step;
                ecount--;
            }
        }
        else
        {
            pt1.y >>= XY_SHIFT;
            
            while( ecount >= 0 )
            {
                ICV_PUT_POINT(pt1.x >> XY_SHIFT, pt1.y);
                pt1.x += x_step;
                pt1.y++;
                ecount--;
            }
        }
        
#undef ICV_PUT_POINT
    }
    else
    {
#define  ICV_PUT_POINT(_x,_y)   \
x = (_x); y = (_y);             \
if( 0 <= x && x < size.width && \
0 <= y && y < size.height ) \
{                               \
tptr = ptr + y*step + x*pix_size;\
for( j = 0; j < pix_size; j++ ) \
tptr[j] = ((uchar*)color)[j]; \
}
        
        ICV_PUT_POINT((pt2.x + (XY_ONE >> 1)) >> XY_SHIFT,
                      (pt2.y + (XY_ONE >> 1)) >> XY_SHIFT);
        
        if( ax > ay )
        {
            pt1.x >>= XY_SHIFT;
            
            while( ecount >= 0 )
            {
                ICV_PUT_POINT(pt1.x, pt1.y >> XY_SHIFT);
                pt1.x++;
                pt1.y += y_step;
                ecount--;
            }
        }
        else
        {
            pt1.y >>= XY_SHIFT;
            
            while( ecount >= 0 )
            {
                ICV_PUT_POINT(pt1.x >> XY_SHIFT, pt1.y);
                pt1.x += x_step;
                pt1.y++;
                ecount--;
            }
        }
        
#undef ICV_PUT_POINT
    }
}
