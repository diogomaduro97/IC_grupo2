#include <stdio.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <map>
#include <math.h>
using namespace cv;
using namespace std;
#define LOG_BASE log(2)
#define WAIT_KEY 0          //0 para imagem e 1 para video
#define COLOR_CHANNELS 4
#define IMG_COLOR -1
#define HISTO_BITSCAPTURE 1 // quanto maior o numero menos pixeis sao contados para o hitograma, mais rapida e a captura de ecra
#define HISTO_WINDOWSIZE_Y 1000
#define HISTO_WINDOWSIZE_X 1920
#define HISTO_SIZE 530
#define RECTANGLE_SIZE 2 // quanto maior o numero menor o tamanho do rectagulo
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
                short temp = image.at<Vec4b>(r,c)[channel];            
                // cout << image.at<Vec3b>(r,c)[i] << ",";
                histo[channel][temp] = histo[channel][temp] + HISTO_BITSCAPTURE;
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
        for(it = histo[i].begin(); it!=histo[i].end() ; it++ ){
            rectangle(histogram_image, Point((HISTO_SIZE*i) + 2*it->first, histogram_image.rows - (it->second >>RECTANGLE_SIZE)), 
                Point((HISTO_SIZE*i) + 2*(it->first+1), histogram_image.rows),Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0));
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
        for(it = histo[i].begin(); it!=histo[i].end() ; it++ ){
            double prob = (double)it->second/((double)sample_size*HISTO_BITSCAPTURE);
            entropy[i] = entropy[i] - prob*log(prob);
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
    Mat imagemout(image.rows, image.cols,CV_8UC3, Scalar(0,0,0));
    string file = "histo.txt";

    for (int r = 0; r < image.rows; r++){
        for (int c = r%HISTO_BITSCAPTURE; c < image.cols; c=c+HISTO_BITSCAPTURE){
            // cout << "(" << image.at<Vec3b>(r,c) <<") | ";
            for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
                imagemout.at<Vec4b>(r,c)[channel] = image.at<Vec4b>(r,c)[channel] >> 4;            
            }
        }
        // cout << ";" <<endl;
    }
    Mat imagemout2(image.rows, image.cols,CV_8UC3, Scalar(0,0,0));
    

    for (int r = 0; r < image.rows; r++){
        for (int c = r%HISTO_BITSCAPTURE; c < image.cols; c=c+HISTO_BITSCAPTURE){
            for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
                imagemout2.at<Vec4b>(r,c)[channel] = imagemout.at<Vec4b>(r,c)[channel] << 4;            
            }
        }
        // cout << ";" <<endl;
    }
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
    namedWindow("Display ImageOut", WINDOW_AUTOSIZE );
    imshow("Display ImageOut", imagemout);
    namedWindow("Display ImageOut2", WINDOW_AUTOSIZE );
    imshow("Display ImageOut2", imagemout2);
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imwrite("imagemout2.jpg", imagemout2);
    imshow("Display Image", image);
    imwrite("histo.jpg", histo_image);
    imwrite("imagem.jpg", image);
    waitKey(WAIT_KEY);
    
    return 0;
} 
// int main(int argc, char** argv )  // main para usar webcam (meter WAIT_KEY diferente de 0)
// {
//     VideoCapture cap(0);
//     Mat image;
//     while(1)
//     {
//         cap.read(image);

//         vector<map<short,int>> histo = createHistogram(image);

//         vector<double> entropy = histoEntropy(histo,image.rows*image.cols/HISTO_BITSCAPTURE);
//         cout << entropy[0] << endl;
//         Mat histo_image = imageHisto(histo,entropy);
        
//         namedWindow("Histogram", WINDOW_AUTOSIZE );
//         imshow("Histogram", histo_image);
//         namedWindow("Display Image", WINDOW_AUTOSIZE );
//         imshow("Display Image", image);
//         waitKey(WAIT_KEY);
//     }
//     return 0;
// } 
