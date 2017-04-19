#include <iostream>
#include <omp.h>
#include <opencv/cv.h>
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <math.h>
using namespace cv;
using namespace std;
float var(int hist[],int level,float val,int pix_num )
{
    long long total=pix_num*val;
    int n=0;
    long long m=0;
    for(int i=0;i<level;i++)
    {
        m+=i*hist[i];
        n+=hist[i];
    }
    long long rem=total-m;
    int rempix=pix_num-n;
    float w0=(1.0*n)/(1.0*pix_num);
    float w1=(1.0*rem)/(1.0*pix_num);
    float u0=(1.0*m)/(1.0*n);
    float u1=(1.0*rem)/(1.0*rempix);
    return w0*w1*(u0-u1)*(u0-u1);
}
int main()
{
    Mat img;
    string name="edited.png";
    img=imread(name);
    cvtColor(img,img,CV_RGB2GRAY);
    long long u=0;
    int hist[256];
    for(int i=0;i<256;i++)
        hist[i]=0;
    int sz=img.cols*img.rows;
    for (int i=0;i<img.rows;i++)
    {
        for(int j=0;j<img.cols;j++)
        {
            int n=img.at<uchar>(i,j);
            u+=n;
            hist[n]++;
        }
    }
    int pix_num=img.rows*img.cols;
    float val=(1.0*u)/float(pix_num);
    float max=0;
    int threshold=0;
    for(int i=1;i<255;i++)
    {
        int x=var(hist,i,val,pix_num);
        if(x>max)
        {
            max=x;
            threshold=i;
        }
    }
    for(int i=0;i<img.rows;i++)
    {
        for(int j=0;j<img.cols;j++)
        {
            if(img.at<uchar>(i,j)>threshold)
            {
                img.at<uchar>(i,j)=255;
            }
            else
                img.at<uchar>(i,j)=0;
        }
    }
    imwrite("otsu.png",img);
    waitKey(5000);
    return 0;
}
