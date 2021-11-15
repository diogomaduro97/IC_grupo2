#include <stdio.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include <map>
using namespace cv;
using namespace std;

#define WAIT_KEY 0          //0 para imagem e 1 para video
#define COLOR_CHANNELS 3
#define IMG_COLOR -1

int main(int argc, char* argv[]) {

    const char* fileIn = argc > 1? argv[1]:"../img.jpg";
    const char* fileOut = argc > 2? argv[2]:"../newImage.jpg";

    Mat img = imread(fileIn, IMG_COLOR);
    if (img.empty()) {
        cout << "Load image failed. Can't find image" << endl;
    }
    Mat newImg(img.rows, img.cols, CV_8UC3, Scalar(0,0,0));

    for (int r = 0; r < img.rows; r++) {
        for (int c = 0; c < img.cols; c++) {
            Vec3b bgrPixel = img.at<Vec3b>(r,c);
            newImg.at<Vec3b>(r,c) = bgrPixel;

        }
    }
    
    imwrite(fileOut, newImg);
    namedWindow("New image", WINDOW_AUTOSIZE);
    imshow("New image", newImg);
    waitKey(WAIT_KEY);
    
    return 0;
} 
