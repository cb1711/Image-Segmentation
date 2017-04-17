#include <iostream>
#include <omp.h>
#include <opencv/cv.h>
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <math.h>
#define THRESHOLD 20
using namespace std;
using namespace cv;
typedef struct centroid
{
    int r,g,b,counter;
}centroid;
/*Smoothen the histogram by making the value equal to
 the mean of all the values in a window of size 5*/
void smoothen(int histogram[],int mhistogram[])
{
    for(int i=0;i<2;i++)
    {
        mhistogram[i]=histogram[i];
        mhistogram[255-i]=histogram[255-i];
    }
    for(int i=2;i<254;i++)
    {
        mhistogram[i]=(histogram[i-2]+histogram[i-1]+histogram[i]+histogram[i+1]+histogram[i+2])/5;
    }
}
/*Calculate the distance of pixel from the centroid*/
float distance(int r,int g,int b,centroid cent)
{
    return sqrt(float((r-cent.r)*(r-cent.r)+(g-cent.g)*(g-cent.g)+(b-cent.b)*(b-cent.b)));
}
/*Return the indexes of the peaks in the histogram*/
vector<int> peak_finder(int histogram[],int x,int y)
{
    vector<int> peaks;
    if(histogram[0]>histogram[1])
        peaks.push_back(0);
    for(int i=1;i<255;i++)
    {
        if(histogram[i]>histogram[i-1] && histogram[i]>histogram[i+1])
        {
            if(histogram[i]>(x*y)/400)
                peaks.push_back(i);
        }
    }
    if(histogram[254]<histogram[255])
        peaks.push_back(255);
    return peaks;
}

/*return all the possible centroids*/
vector <centroid> centroid_finder(vector <int> &rpeaks,vector <int> &gpeaks,vector <int> &bpeaks)
{
    int rs=rpeaks.size();
    int bs=bpeaks.size();
    int gs=gpeaks.size();
    vector <centroid> centroids;
    for(int i=0;i<rs;i++)
    {
        int r=rpeaks[i];
        for(int j=0;j<bs;j++)
        {
            int b=bpeaks[j];
            for(int k=0;k<gs;k++)
            {
                int g=gpeaks[k];
                centroid c;
                c.b=b;
                c.r=r;
                c.g=g;
                c.counter=0;
                centroids.push_back(c);
            }
        }
    }
    return centroids;
}
/*Assign the pixels to the centroids*/
void assign_centroid(vector<centroid> &centroids,Mat &image,vector <int> &parents)
{
    int size=centroids.size();
    int x=image.rows;
    int y=image.cols;
    #pragma omp parallel for schedule(dynamic,x/20)
    for(int i=0;i<x;i++)
    {
        for(int j=0;j<y;j++)
        {
            float nearest;
            int ni;
            for(int k=0;k<size;k++)
            {
                int r=image.at<Vec3b>(i,j)[2];
                int g=image.at<Vec3b>(i,j)[1];
                int b=image.at<Vec3b>(i,j)[0];
                float dist=distance(r,g,b,centroids[k]);
                if(k==0)
                {
                    nearest=dist;
                    ni=k;
                }
                if(dist<nearest)
                {
                    nearest=dist;
                    ni=k;
                }

            }
            parents[i*y+j]=ni;
            #pragma omp atomic
            centroids[ni].counter++;
        }
    }
}
/*Remove the centroids having lesser number of elements*/
vector <centroid> revise(vector <centroid> &centroids,int x,int y)
{
    int size=centroids.size();
    vector <centroid> revised;
    int threshold=(x*y*10)/1000;
    for(int i=0;i<size;i++)
    {
        if(centroids[i].counter>threshold)
        {
            centroids[i].counter=0;
            revised.push_back(centroids[i]);
        }
    }
    return revised;
}
int main()
{
    string name="nebullfa.jpg";
    Mat image;
    image=imread(name);
    int x=image.rows;
    int y=image.cols;
    int r_hist[256];
    int b_hist[256];
    int g_hist[256];
    int rh[4][256]={{0},{0},{0},{0}};
    int bh[4][256]={{0},{0},{0},{0}};
    int gh[4][256]={{0},{0},{0},{0}};
    int rm_hist[256];
    int bm_hist[256];
    int gm_hist[256];
    vector <int> rpeaks;
    vector <int> bpeaks;
    vector <int> gpeaks;
    //Make the histogram
    #pragma omp parallel
    {
    #pragma omp for schedule(dynamic,x/4)
    for(int i=0;i<x;i++)
    {
        int id=omp_get_thread_num();
        for(int j=0;j<y;j++)
        {
            int rval=image.at<Vec3b>(i,j)[2];
            int gval=image.at<Vec3b>(i,j)[1];
            int bval=image.at<Vec3b>(i,j)[0];
            rh[id][rval]++;
            bh[id][bval]++;
            gh[id][gval]++;
        }
    }
    #pragma omp for schedule(dynamic,64)
    for(int i=0;i<256;i++)
    {
        r_hist[i]=rh[0][i]+rh[1][i]+rh[2][i]+rh[3][i];
        g_hist[i]=gh[0][i]+gh[1][i]+gh[2][i]+gh[3][i];
        b_hist[i]=bh[0][i]+bh[1][i]+bh[2][i]+bh[3][i];
    }
    #pragma omp  sections
    {
        #pragma omp section
            smoothen(r_hist,rm_hist);
        #pragma omp section
            smoothen(g_hist,gm_hist);
        #pragma omp section
             smoothen(b_hist,bm_hist);
    }
    #pragma omp sections
    {
        #pragma omp section
            rpeaks=peak_finder(rm_hist,x,y);
        #pragma omp section
            gpeaks=peak_finder(gm_hist,x,y);
        #pragma omp section
            bpeaks=peak_finder(bm_hist,x,y);
    }
    }
    vector <int> parents(x*y);
    vector <centroid> centroids=centroid_finder(rpeaks,gpeaks,bpeaks);
    assign_centroid(centroids,image,parents);
     cout<<"assign"<<endl;
    vector<centroid> revCentroids=revise(centroids,x,y);
    centroids.clear();
    cout<<revCentroids.size();
    assign_centroid(revCentroids,image,parents);
    #pragma omp parallel for schedule(dynamic,x/8)
    for(int i=0;i<x;i++)
    {
        for(int j=0;j<y;j++)
        {
            image.at<Vec3b>(i,j)[0]=revCentroids[parents[i*y+j]].b;
            image.at<Vec3b>(i,j)[1]=revCentroids[parents[i*y+j]].g;
            image.at<Vec3b>(i,j)[2]=revCentroids[parents[i*y+j]].r;
        }
    }
    imwrite("edited_histp.png",image);
    //resize(image,image,cvSize(600,400));
    waitKey(5000);
}
