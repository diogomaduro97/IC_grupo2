#include <stdio.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <map>
using namespace cv;
using namespace std;

#define WAIT_KEY 1          //0 para imagem e 1 para video
#define COLOR_CHANNELS 3
#define IMG_COLOR -1
#define HISTO_BITSCAPTURE 5 // quanto maior o numero menos pixeis sao contados para o hitograma, mais rapida e a captura de ecra
#define HISTO_WINDOWSIZE_Y 1080
#define HISTO_WINDOWSIZE_X 1920
#define HISTO_SIZE 530
#define HISTO_NUMCOLLUMS 1 //quanto maior for o numero, menos colunas aparecem
map<short,int>::iterator it;
vector<map<short,int>> createHistogram(Mat image){
    vector<map<short,int>> histo;
    for(int i=0 ; i < COLOR_CHANNELS; i++){
        map<short,int> mapa;
        histo.push_back(mapa);
    }
    for (int r = 0; r < image.rows; r++)
    {
        for (int c = r%HISTO_BITSCAPTURE; c < image.cols; c=c+HISTO_BITSCAPTURE)
        {
            for (size_t channel = 0; channel < COLOR_CHANNELS; channel++)
            {
                short temp = image.at<Vec3b>(r,c)[channel];            
                // cout << image.at<Vec3b>(r,c)[i] << ",";
                if(it == histo[channel].end()) histo[channel][temp]=0;
                histo[channel][temp]++;
            }
            // cout << "(" << image.at<Vec3b>(r,c) <<") | ";
        }
        // cout << ";" <<endl;
    }
    return histo;
}
void histoToFile(string file, vector<map<short,int>> histo){
    ofstream ofs(file);
    for(int i = 0; i< COLOR_CHANNELS; i++){
        ofs << "canal numero: " << i << endl;
        for(it = histo[i].begin(); it!=histo[i].end() ; it++ ){
            ofs<< it->first << " : " << it->second << endl;

        }
    }

}
Mat imageHisto(vector<map<short,int>> histo){
    Mat histogram_image(HISTO_WINDOWSIZE_Y, HISTO_WINDOWSIZE_X,CV_8UC3, Scalar(0,0,0));

    for (int i = 0; i < histo.size(); i++)
    {
        for (int value = 0; value < histo[i].size(); value=value+HISTO_NUMCOLLUMS)
        {
            rectangle(histogram_image, Point((HISTO_SIZE*i) + 2*value, histogram_image.rows - (histo[i][value] >> 1)), 
                Point((HISTO_SIZE*i) + 2*(value+1), histogram_image.rows),Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0));
        }
         
    }
    
    return histogram_image;
}
int main(int argc, char** argv )
{
    /* 
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    } */
    VideoCapture cap(0);
    Mat image;
    // Mat image = imread("gaja.jpg", IMG_COLOR );
    // string file = "histo.txt";
    while(1)
    {
        cap.read(image);

        vector<map<short,int>> histo = createHistogram(image);
        // histoToFile(file,histo);
        Mat histo_image = imageHisto(histo);
        
        namedWindow("Histogram", WINDOW_AUTOSIZE );
        imshow("Histogram", histo_image);

        if ( !image.data )
        {
            printf("No image data \n");
            return -1; 
        }
        namedWindow("Display Image", WINDOW_AUTOSIZE );
        imshow("Display Image", image);
        waitKey(WAIT_KEY);
    }
    return 0;
} 





/* int main(){
    VideoCapture cap(0);
    Mat image;
    Mat imgGrey, imgGranny;
    while(1){
        
        cap.read(image);
        cvtColor(image, imgGrey ,COLOR_BGR2GRAY);
        Canny(imgGrey,imgGranny,150,200);
        imshow("Image", image);
        imshow("Image Grey", imgGrey);
        
        imshow("Image Canny", imgGranny);
        waitKey(20);
    }
} */