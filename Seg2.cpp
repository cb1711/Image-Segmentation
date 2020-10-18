//Serialized implementation for image segmentation using Boruvka's algorithm

// Boruvka's Algorithm is a greedy algorithm for finding a minimum spanning tree in a connected graph


#include <iostream>
#include <omp.h>
#include <opencv/cv.h>
#include "opencv2/highgui/highgui.hpp"
#include <vector>
using namespace std;
using namespace cv;

// Structure containing coordinates of any point
struct pixel{
    int x;
    int y;
};
bool use=false;
typedef struct un un;
typedef struct pixel pixel;

// Structure defining an edge with the two end points and weight of the edge
struct edge{
    float weigh;
    pixel node1;
    pixel node2;
};
typedef struct edge edge;
float weight(int p1[],int p2[])
{
    float weight=(p1[0]-p2[0])*(p1[0]-p2[0])+(p1[1]-p2[1])*(p1[1]-p2[1])+(p1[2]-p2[2])*(p1[2]-p2[2]);
    weight=sqrt(weight);
    return weight;
}
int root (vector <int> &Arr ,int i)
{

    while(Arr[i]!= i)
    {
        Arr[i] = Arr[Arr[i]] ;
        i = Arr[i];
    }
    return i;
}
int main()
{
    Mat img1;
    float k=30;
    string name="data.jpg";

    img1=imread(name);
    //resize(img1,img1,cvSize(1920,1080));
    int x=img1.rows;
    int y=img1.cols;
    cout<<x<<" "<<y<< " ";
    int sz=2*x*y-x-y;
    vector <edge> edges;
    vector <int> Array(x*y);
    vector <int> Size(x*y);
    vector <int> cheapest(x*y);
    vector <float> Earray(x*y);
    int chunk=(y)/16;

    for(int i=0;i<x;i++)
    {
        //#pragma omp for schedule(dynamic,chunk)
        for(int j=0;j<y;j++)
        {
            if(j==y-1 && i==x-1)
                continue;
            else if(j==y-1)
            {
                int t1[3];
                int t2[3];
                t1[0]=img1.at<Vec3b>(i,j)[0];
                t1[1]=img1.at<Vec3b>(i,j)[1];
                t1[2]=img1.at<Vec3b>(i,j)[2];
                t2[0]=img1.at<Vec3b>(i+1,j)[0];
                t2[1]=img1.at<Vec3b>(i+1,j)[1];
                t2[2]=img1.at<Vec3b>(i+1,j)[2];
                edge e1;
                e1.weigh=weight(t1,t2);
                e1.node1.x=i;
                e1.node1.y=j;
                e1.node2.x=i+1;
                e1.node2.y=j;
                edges.push_back(e1);
            }
            else if(i==x-1)
            {
                int t1[3];
                int t2[3];
                t1[0]=img1.at<Vec3b>(i,j)[0];
                t1[1]=img1.at<Vec3b>(i,j)[1];
                t1[2]=img1.at<Vec3b>(i,j)[2];
                t2[0]=img1.at<Vec3b>(i,j+1)[0];
                t2[1]=img1.at<Vec3b>(i,j+1)[1];
                t2[2]=img1.at<Vec3b>(i,j+1)[2];
                edge e1;
                e1.weigh=weight(t1,t2);
                e1.node1.x=i;
                e1.node1.y=j;
                e1.node2.x=i;
                e1.node2.y=j+1;
                edges.push_back(e1);
            }
            else{
                int t1[3];
                int t2[3];
                t1[0]=img1.at<Vec3b>(i,j)[0];
                t1[1]=img1.at<Vec3b>(i,j)[1];
                t1[2]=img1.at<Vec3b>(i,j)[2];
                t2[0]=img1.at<Vec3b>(i+1,j)[0];
                t2[1]=img1.at<Vec3b>(i+1,j)[1];
                t2[2]=img1.at<Vec3b>(i+1,j)[2];
                edge e1;
                e1.weigh=weight(t1,t2);
                e1.node1.x=i;
                e1.node1.y=j;
                e1.node2.x=i+1;
                e1.node2.y=j;
               // cout<<(2*(i*y+j))-i;
                edges.push_back(e1);
                t2[0]=img1.at<Vec3b>(i,j+1)[0];
                t2[1]=img1.at<Vec3b>(i,j+1)[1];
                t2[2]=img1.at<Vec3b>(i,j+1)[2];
                e1.weigh=weight(t1,t2);
                e1.node2.x=i;
                e1.node2.y=j+1;
                edges.push_back(e1);
                //cout<<"e";
            }
        }
    }
    double time=omp_get_wtime();
    #pragma omp for schedule(dynamic,x*y/30)
    for(int i=0;i<x*y;i++)
    {
        Array[i]=i;
        Size[i]=1;
        cheapest[i]=-1;
        Earray[i]=-1;
        //omp_init_lock(&locks[i]);
    }

    int t=x*y;
    int counter=t;
    int flag=0;
    int temp;
    //cout<<2*x*y-x-y<<endl;
//    for(int i=0;i<x*y;i++)
//        lockarr[i]=false;
    while(flag==0)
    {
        t=counter;
        //#pragma omp parallel for schedule(dynamic,sz/16)
        for(int i=0;i<sz;i++)
        {
            int a=edges[i].node1.x;
            int b=edges[i].node1.y;
            int c=edges[i].node2.x;
            int d=edges[i].node2.y;
            a=a*y+b;
            c=c*y+d;
            int ra,rc;
            ra=root(Array,a);
            rc=root(Array,c);
            if(ra==rc)
                continue;
            else
            {
                if(cheapest[ra]==-1 || edges[cheapest[ra]].weigh>edges[i].weigh)
                {
                    //#pragma omp atomic write
                    //#pragma omp critical
                    cheapest[ra]=i;
                    //omp_unset_lock(&locks[ra]);
                }
                if(cheapest[rc]==-1 || edges[cheapest[rc]].weigh>edges[i].weigh)
                  {
                    //#pragma omp critical
                    //#pragma omp atomic write
                    //#pragma omp critical
                    cheapest[rc]=i;
                    //omp_unset_lock(&locks[rc]);
                  }
            }
        }
        temp=0;
        //#pragma omp parallel for schedule(dynamic,x*y/8) reduction(+:temp)
        //#pragma omp master

        for(int i=0;i<x*y;i++)
        {
            if(cheapest[i]!=-1)
            {
                edge b=edges[cheapest[i]];
                int a=b.node1.x*y+b.node1.y;
                int c=b.node2.x*y+b.node2.y;
//            while(lockarr[a] && lockarr[c])
//               continue;
//            #pragma omp critical
//            {
//                lockarr[a]=true;
//               lockarr[c]=true;
//            }
//            #pragma omp critical
//            {
//                omp_set_lock(&locks[a]);
//                omp_set_lock(&locks[c]);
//            }
            //cout<<"Hello";
            int ra=root(Array,a);
            int rb=root(Array,c);
            if(ra==rb)
            {
//                omp_unset_lock(&locks[a]);
//                omp_unset_lock(&locks[c]);
                continue;
            }
            if (Earray[ra]==-1)
                Earray[ra]=b.weigh;
            if (Earray[rb]==-1)
                Earray[rb]=b.weigh;
            float T1=float(Earray[ra])+k/float(Size[a]);
            float T2=float(Earray[rb])+k/float(Size[c]);
            float min=T1>T2?T2:T1;
            //condition for merging the two trees
            if (b.weigh<=min)
            {
            //union
                if(Size[ra]>Size[rb])
                {
                    Size[ra]+=Size[rb];
                    Array[rb]=ra;
                    if(edges[cheapest[i]].weigh>Earray[ra])
                        Earray[ra]=edges[cheapest[i]].weigh;
                    if (Earray[ra]<=Earray[rb])
                        Earray[ra]=Earray[rb];
                    cheapest[ra]=-1;
                    //cout<<"df";
                }
                else
                {
                    Size[rb]+=Size[ra];
                    Array[ra]=rb;
                    if(edges[cheapest[i]].weigh>Earray[rb])
                        Earray[rb]=edges[cheapest[i]].weigh;
                    if (Earray[rb]<=Earray[ra])
                        Earray[rb]=Earray[ra];
                    cheapest[rb]=-1;
                }
                temp++;
             }
//           omp_unset_lock(&locks[a]);
//           omp_unset_lock(&locks[c]);
            }
        }
    counter-=temp;
    if(t==counter)
        flag=1;
    }
    #pragma omp parallel for schedule(dynamic,chunk)
     for(int i=0;i<x*y;i++)
    {
        int xcoord=i/y;
        int ycoord=i%y;
        int rc=root(Array,i);
        int rcx=rc/y;
        int rcy=rc%y;
        img1.at<Vec3b>(xcoord,ycoord)[0]=img1.at<Vec3b>(rcx,rcy)[0];
        img1.at<Vec3b>(xcoord,ycoord)[2]=img1.at<Vec3b>(rcx,rcy)[2];
        img1.at<Vec3b>(xcoord,ycoord)[1]=img1.at<Vec3b>(rcx,rcy)[1];
//        if(rc==root(Array,0))
//        {
//            img1.at<Vec3b>(xcoord,ycoord)[0]=0;
//        img1.at<Vec3b>(xcoord,ycoord)[2]=0;
//        img1.at<Vec3b>(xcoord,ycoord)[1]=0;
//        }
    }
    time=omp_get_wtime()-time;
    cout<<time;
    imwrite( "edited.png", img1 );
    //resize(img1,img1,cvSize(700,800));
    imshow("image",img1);
    waitKey(5000);
    return 0;
}
