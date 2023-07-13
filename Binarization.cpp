#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <stdio.h>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <math.h>
using namespace std;
using namespace cv;

float fastMean(Mat& integral, int x, int y, int window_half_length)
{

    int min_y = max(0, y - window_half_length );
    int max_y = min(integral.rows - 1, y + window_half_length );
    int min_x = max(0, x - window_half_length);
    int max_x = min(integral.cols - 1, x + window_half_length );

    int topright = integral.at<int>(max_y, max_x);
    int botleft = integral.at<int>(min_y, min_x);
    int topleft = integral.at<int>(max_y, min_x);
    int botright = integral.at<int>(min_y, max_x);

    float res = (float)((topright + botleft - topleft - botright) / (float)((max_y - min_y) * (max_x - min_x)));

    return res;
}



float slowMean(Mat& inputimg, int x, int y, int window_half_length)
{

    float tempsum = 0;
    for (int i = 0; i < 2 * window_half_length + 1; i++)
    {
        for (int j = 0; j < 2 * window_half_length + 1; j++)
        {
            int pixel;
            if ((y - window_half_length + i <= 0) || (x - window_half_length + j <= 0))
            {
                continue;
            }
            else if ((y - window_half_length + i > inputimg.size[1]) || (x - window_half_length + j > inputimg.size[0]))
            {
                continue;
            }
            
            pixel = (int)inputimg.at<uchar>(y - window_half_length + i, x - window_half_length + j);
            
            tempsum += pixel;
        }
    }

    float res = (float)(tempsum / (float)(powf(2 * window_half_length + 1, 2)));

    return res;
}

float STD(Mat& InputImg, int x, int y, int window_half_length, float mean)
{
    float tempsum = 0;

    for (int i = 0; i < 2 * window_half_length + 1; i++)
    {
        for (int j = 0; j < 2 * window_half_length + 1; j++)
        {
            int pixel;
            //cout << "program arrived here" << endl;
            if ((y - window_half_length + i <= 0) || (x - window_half_length + j <= 0))
            {
                continue;
            }
            else if ((y - window_half_length + i > InputImg.size[1]) || (x - window_half_length + j > InputImg.size[0]))
            {
                continue;
            }
            pixel = (int)InputImg.at<uchar>(y - window_half_length + i, x - window_half_length + j);
            //cout << "program at done" << endl;
            tempsum += powf(pixel - mean, 2);
        }
    }
    //cout << "tempsum is " << tempsum << endl;
    //cout << "(powf((2 * window_half_length + 1), 2)) is " << (powf((2 * window_half_length + 1), 2)) << endl;
    float std = powf(tempsum / (powf((2 * window_half_length + 1), 2)), 0.5);

    return std;
}

//int Min_Avg_Pixel_Black(Mat inpImg)
//{
//
//}

const int min_avg_pixel_black = 60;

Mat Sauvola(Mat inpImg, int window_half_length, float k)
{
    cout << "here is Sauvola" << endl;
    Mat integral;
    Mat resImg;
    inpImg.copyTo(resImg);
    int nYOffSet = window_half_length;
    int nXOffSet = window_half_length;
    cv::integral(inpImg, integral);
    for (int y = 0; y < inpImg.rows; y += nYOffSet)
    {
        for (int x = 0; x < inpImg.cols; x += nXOffSet)
        {
            //float fmean = fastMean(integral, x, y, window_half_length);
            float fmean = slowMean(inpImg, x, y, window_half_length);
            //cout << "done fastmean" << endl;
            float std;
            std = STD(inpImg, x, y, window_half_length, fmean);
            //cout << "done STD" << endl;
            float fthreshold = (float)(fmean * (1 + k * (std / 128 - 1)));
            
            //if (x % 20 == 0) { cout << "x is " << x << endl << "fthreshold is " << fthreshold << endl << "fmean is :" << fmean << endl << "std is :" << std << endl;}
            int nNextY = y + nYOffSet;
            int nNextX = x + nXOffSet;
            int nCurY = y;
            while (nCurY < nNextY && nCurY < inpImg.rows)
            {
                //cout << "d1" << endl;
                int nCurX = x;
                while (nCurX < nNextX && nCurX < inpImg.cols)
                {
                    //cout << "d2" << endl;
                    uchar val = inpImg.at<uchar>(nCurY, nCurX);
                    if (val <= min_avg_pixel_black)
                    {
                        resImg.at<uchar>(nCurY, nCurX) = 0;
                    }
                    else if (val <= fthreshold)
                    {
                        resImg.at<uchar>(nCurY, nCurX) = 0;
                    }
                    else
                    {
                        resImg.at<uchar>(nCurY, nCurX) = 255;
                    }
                    //if (val == 0) { cout << "%%%%%%%%%%%%%" << endl; }
                    //cout << "d3" << endl;
                    //resImg.at<uchar>(nCurY, nCurX) = (val == 0 ? 255: 0);
                    //cout << "d4" << endl;
                    nCurX++;
                }
                nCurY++;
            }
        }
    }
    cout << "Sauvola done" << endl;
    cout << "Sauvola return with size:" << resImg.size << endl;

    return resImg;
}
