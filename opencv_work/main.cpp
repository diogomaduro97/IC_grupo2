#include <stdio.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <map>
#include <math.h>
using namespace cv;
using namespace std;

#define WAIT_KEY 0          //0 para imagem e 1 para video
#define COLOR_CHANNELS 3
#define IMG_COLOR -1
#define HISTO_BITSCAPTURE 1 // quanto maior o numero menos pixeis sao contados para o hitograma, mais rapida e a captura de ecra
#define HISTO_WINDOWSIZE_Y 1000
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
    for (int r = 0; r < image.rows; r++){
        for (int c = r%HISTO_BITSCAPTURE; c < image.cols; c=c+HISTO_BITSCAPTURE){
            for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
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

Mat imageHisto(vector<map<short,int>> histo,vector<double> entropy){
    Mat histogram_image(HISTO_WINDOWSIZE_Y, HISTO_WINDOWSIZE_X,CV_8UC3, Scalar(0,0,0));

    for (int i = 0; i < histo.size(); i++){
        for (int value = 0; value < histo[i].size(); value=value+HISTO_NUMCOLLUMS){
            rectangle(histogram_image, Point((HISTO_SIZE*i) + 2*value, histogram_image.rows - (histo[i][value] >> 3)), 
                Point((HISTO_SIZE*i) + 2*(value+1), histogram_image.rows),Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0));
        }
        const string toprint = to_string(entropy[i]);
        putText(histogram_image, toprint , Point(250 + (HISTO_SIZE*i) , 200), FONT_HERSHEY_COMPLEX_SMALL ,0.8, Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0));
    }
    return histogram_image;
}
vector<double> histoEntropy(vector<map<short,int>> histo, int sample_size ){
    vector<double> entropy;
    for(int i=0 ; i < COLOR_CHANNELS; i++){
        double tmp = 0;
        entropy.push_back(tmp);
    }
    for(int i = 0; i < histo.size(); i++){
        for (int j = 0; j < histo[i].size(); j++){
            double prob = (double)histo[i][j]/(double)sample_size;
            entropy[i] = entropy[i] - prob*((prob ? log(prob) : 0)/log(histo[i].size()));
        }
    }
    return entropy;
}
int main(int argc, char** argv ) // main para usar imagens (meter WAIT_KEY a 0)
{
    /* 
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    } */
    Mat image = imread("cao.jpg", IMG_COLOR );
    string file = "histo.txt";

    vector<map<short,int>> histo = createHistogram(image);
    histoToFile(file,histo);

    vector<double> entropy = histoEntropy(histo,image.rows*image.cols/HISTO_BITSCAPTURE);
    Mat histo_image = imageHisto(histo,entropy);
    
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
    
    return 0;
} 
// int main(int argc, char** argv )  // main para usar webcam (meter WAIT_KEY diferente de 0)
// {
//     /* 
//     if ( argc != 2 )
//     {
//         printf("usage: DisplayImage.out <Image_Path>\n");
//         return -1;
//     } */
//     VideoCapture cap(0);
//     Mat image;
//     // Mat image = imread("cao.jpg", IMG_COLOR );
//     // string file = "histo.txt";
//     while(1)
//     {
//         cap.read(image);

//         vector<map<short,int>> histo = createHistogram(image);
//         // histoToFile(file,histo);

//         vector<double> entropy = histoEntropy(histo,image.rows*image.cols/HISTO_BITSCAPTURE);
//         cout << entropy[0] << endl;
//         Mat histo_image = imageHisto(histo,entropy);
        
//         namedWindow("Histogram", WINDOW_AUTOSIZE );
//         imshow("Histogram", histo_image);
//         if ( !image.data )
//         {
//             printf("No image data \n");
//             return -1; 
//         }
//         namedWindow("Display Image", WINDOW_AUTOSIZE );
//         imshow("Display Image", image);
//         waitKey(WAIT_KEY);
//     }
//     return 0;
// } 





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