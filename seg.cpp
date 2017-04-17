#include <iostream>
#include <omp.h>
#include <opencv/cv.h>
#include "opencv2/highgui/highgui.hpp"
#include <vector>
using namespace std;
using namespace cv;
struct pixel{
    int x;
    int y;
};
bool use=false;
typedef struct un un;
typedef struct pixel pixel;
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
    int np=4;
    float k=30/np+1;
    string name="nebullfa.jpg";
    img1=imread(name);
    //resize(img1,img1,cvSize(1920,1080));
    //resize(img1,img1,cvSize(500,700));
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
                edges.push_back(e1);
                t2[0]=img1.at<Vec3b>(i,j+1)[0];
                t2[1]=img1.at<Vec3b>(i,j+1)[1];
                t2[2]=img1.at<Vec3b>(i,j+1)[2];
                e1.weigh=weight(t1,t2);
                e1.node2.x=i;
                e1.node2.y=j+1;
                edges.push_back(e1);
            }
        }
    }
    cout<<"hello world"<<endl;
    double time=omp_get_wtime();
    chunk=y/16;
    cout<<sz<<" ";
    #pragma omp parallel for schedule(dynamic,x*y/30)
    for(int i=0;i<x*y;i++)
    {
        Array[i]=i;
        Size[i]=1;
        cheapest[i]=-1;
        Earray[i]=-1;
    }
    int l=2*y-1;
    #pragma omp parallel
    {
        #pragma omp for
        for(int p=0;p<np;p++)
        {
            int t=(x*y)/np-y;
            int counter=t;
            int temp;
            int flag=0;
            int lim;
            lim=(p+1)*(x)/np*l-l;
            int ini=(p*x*l)/np;
            if(p==np-1)
                lim=sz;
            cout<<" "<<lim<<endl;
            cout<<ini<<endl;
            while(flag==0)
            {
                t=counter;
                //#pragma omp parallel for schedule(dynamic,sz/16)
                for(int i=ini;i<lim;i++)
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
                            cheapest[ra]=i;
                        if(cheapest[rc]==-1 || edges[cheapest[rc]].weigh>edges[i].weigh)
                            cheapest[rc]=i;
                    }
                }
                temp=0;
                int starter=(x*y)/np*p;
                int ender=((x*y)/np)*(p+1);
                if (p==np-1)
                    ender=x*y;
                for(int i=starter;i<ender;i++)
                {
                    if(cheapest[i]!=-1)
                    {
                        edge b=edges[cheapest[i]];
                        int a=b.node1.x*y+b.node1.y;
                        int c=b.node2.x*y+b.node2.y;

                        int ra=root(Array,a);
                        int rb=root(Array,c);
                        if(ra==rb)
                            continue;
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
                    }
                }
                counter-=temp;
                if(t==counter)
                    flag=1;
            }
        }
    }
    //to merge the image segments made by each processor separately
    #pragma omp parallel for
    for(int i=0;i<np-1;i++)
    {
        for(int j=0;j<y;j++)
        {
            int tomerge=((i+1)*x/np-1)*l+2*j;
            edge b=edges[tomerge];
            int a=b.node1.x*y+b.node1.y;
            int c=b.node2.x*y+b.node2.y;
            int ra=root(Array,a);
            int rb=root(Array,c);
            if(ra==rb)
                continue;
            if (Earray[ra]==-1)
                Earray[ra]=b.weigh;
            if (Earray[rb]==-1)
                Earray[rb]=b.weigh;
            float T1=float(Earray[ra])+k/float(Size[ra]);
            float T2=float(Earray[rb])+k/float(Size[rb]);
            float min=T1>T2?T2:T1;
            if (b.weigh<=min/2)
            {
            //union
                if(Size[ra]>Size[rb])
                {
                    Size[ra]+=Size[rb];
                    Array[rb]=ra;
                }
                else
                {
                    Size[rb]+=Size[ra];
                    Array[ra]=rb;
                    }
                }
        }
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
//            img1.at<Vec3b>(xcoord,ycoord)[2]=0;
//            img1.at<Vec3b>(xcoord,ycoord)[1]=0;
//        }
    }
    time=omp_get_wtime()-time;
    cout<<time;
    imwrite( "editedp.png", img1 );
    //resize(img1,img1,cvSize(500,800));
    //imshow("image",img1);
    waitKey(5000);
    return 0;
}
