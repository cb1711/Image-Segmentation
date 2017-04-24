#include<bits/stdc++.h>
#include "opencv2/highgui/highgui.hpp"
#include <opencv/cv.h>
#include <omp.h>
using namespace std;
using namespace cv;
#define ll                  long long
#define rep(i,n)            for(int i=0;i<n;i++)
#define hell                1000000007LL
#define vi                  vector<int>
#define vii                 vector< vi >
#define pb                  push_back
#define mp                  make_pair
#define fi                  first
#define se                  second
#define pii                 pair<int,int>
#define all(c)              c.begin(),c.end()
#define sz(c)               c.size()
struct Node
{
    ll r,g,b;
};
typedef struct Node node;
int main()
{
    ll N,M;
    string img="kmeans.png";
    Mat image,pq;
    image=imread(img);
    pq=imread(img);
    ll N1=image.rows;
    ll M1=image.cols;
    //resize(image,image,cvSize(N1/2,M1/2));
    //resize(pq,pq,cvSize(N1/2,M1/2));
    N=image.rows;
    M=image.cols;
    vector< vector<node> > arr(N);
    rep(i,N)
    {
        rep(j,M)
        {
            node z;
            z.b=image.at<Vec3b>(i,j)[0];
            z.g=image.at<Vec3b>(i,j)[1];
            z.r=image.at<Vec3b>(i,j)[2];
            arr[i].pb(z);
            //cout<<"("<<z.r<<","<<z.g<<","<<z.r<<") ";
            //arr[i][j].b
        }
        //cout<<endl;
    }
    ll K,ss;
    cin>>K>>ss;
    srand(time(0));
    vector<pii > cluster(K),temp(K);
    vi c(K);
    vector<node> color(K);
    rep(i,K) cluster[i]=mp(rand()%N,rand()%M);
    rep(i,K)
    {
        color[i].r=rand()%255;
        color[i].g=rand()%255;
        color[i].b=rand()%255;
    }
    while(ss--)
    {
        cout<<ss<<endl;
        vector< vector<pii > > mat(N,vector<pii >(M,mp(hell,-1)));
        rep(i,K) temp[i]=mp(0,0),c[i]=1;
        rep(i,N)
        {
            rep(j,M)
            {
                rep(k,K)
                {
                    ll d=(arr[i][j].r-arr[cluster[k].fi][cluster[k].se].r)*(arr[i][j].r-arr[cluster[k].fi][cluster[k].se].r);
                    d+=(arr[i][j].b-arr[cluster[k].fi][cluster[k].se].b)*(arr[i][j].b-arr[cluster[k].fi][cluster[k].se].b);
                    d+=(arr[i][j].g-arr[cluster[k].fi][cluster[k].se].g)*(arr[i][j].g-arr[cluster[k].fi][cluster[k].se].g);
                    //d+=3*((i-cluster[k].fi)*(i-cluster[k].fi)+(j-cluster[k].se)*(j-cluster[k].se));
                    if(d<mat[i][j].fi)
                    {
                        mat[i][j].fi=d;
                        mat[i][j].se=k;
                    }
                }
            }
        }
        rep(i,N)
        {
            rep(j,M)
            {
                ll k=mat[i][j].se;
                temp[k].fi+=i;
                temp[k].se+=j;
                c[k]++;
            }
        }
        rep(i,K) c[i]=max(1,c[i]-1);
        rep(i,K) cluster[i]=mp(temp[i].fi/c[i],temp[i].se/c[i]);
        ll flag=0;
        rep(i,K) if(cluster[i]!=temp[i]) flag=1;
        if(flag==0) break;
        rep(i,N)
        {
            rep(j,M)
            {
                ll k=mat[i][j].se;
                pq.at<Vec3b>(i,j)[0]=color[k].b;
                pq.at<Vec3b>(i,j)[1]=color[k].g;
                pq.at<Vec3b>(i,j)[2]=color[k].r;
            }
        }
    }
    resize(pq,pq,cvSize(500,500));
    imshow("image",pq);
    waitKey(1000000);
    return 0;
}
