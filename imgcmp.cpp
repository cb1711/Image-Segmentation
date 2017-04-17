#include <iostream>
#include <omp.h>
#include <opencv/cv.h>
#include "opencv2/highgui/highgui.hpp"
#include <vector>
using namespace std;
using namespace cv;
int main()
{
    Mat img1,img2,img3;
    string name="edited_histp.png";
    string name2="edited_hist.png";
    img1=imread(name);
    img2=imread(name2);
    img3=imread(name2);
    int y=img1.cols;
    for(int i=0;i<img1.rows;i++)
    {
        #pragma omp parallel for schedule(dynamic,y/16)
        for(int j=0;j<img1.cols;j++)
        {

            if(img1.at<Vec3b>(i,j)[0]==img2.at<Vec3b>(i,j)[0] && img1.at<Vec3b>(i,j)[1]==img2.at<Vec3b>(i,j)[1] && img1.at<Vec3b>(i,j)[2]==img2.at<Vec3b>(i,j)[2])
            {
                img3.at<Vec3b>(i,j)[0]=0;
                img3.at<Vec3b>(i,j)[1]=0;
                img3.at<Vec3b>(i,j)[2]=0;
            }
            else{
                img3.at<Vec3b>(i,j)[0]=255;
                img3.at<Vec3b>(i,j)[1]=255;
                img3.at<Vec3b>(i,j)[2]=255;
            }

        }
    }
    imwrite("diff.png",img3);
    resize(img3,img3,cvSize(500,700));
    imshow("diff",img3);
    waitKey(5000);
    return 0;
}
