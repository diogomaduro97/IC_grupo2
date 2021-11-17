#include <stdio.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <map>
#include <math.h>
using namespace cv;
using namespace std;
#define WAIT_KEY 1          //0 para imagem e 1 para video
#define COLOR_CHANNELS 3    
#define IMG_COLOR -1
#define HISTO_BITSCAPTURE 1 // quanto maior o numero menos pixeis sao contados para o hitograma, mais rapida e a captura de ecra
#define HISTO_WINDOWSIZE_Y 1000
#define HISTO_WINDOWSIZE_X 1920
#define HISTO_SIZE 510      // 
#define RECTANGLE_DIVIDER 2 // quanto maior o numero menor o tamanho do rectagulo
#define SHIFT_BITS 5        // [0..7] qunato bits se filtram por cada pixel 
map<short,int>::iterator it;
vector<map<short,int>> createHistogram(Mat image){
    vector<map<short,int>> histo;
    for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
        map<short,int> mapa;
        histo.push_back(mapa);
        for (int r = 0; r < image.rows; r++){
            for (int c = r%HISTO_BITSCAPTURE; c < image.cols; c=c+HISTO_BITSCAPTURE){
                short temp = image.at<Vec3b>(r,c)[channel];            
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
Mat imageHisto(vector<map<short,int>> histo,vector<double> entropy,uint8_t ractangle_divider = RECTANGLE_DIVIDER){
    Mat histogram_image(HISTO_WINDOWSIZE_Y, HISTO_WINDOWSIZE_X,CV_8UC3, Scalar(0,0,0));

    for (int i = 0; i < histo.size(); i++){
        for(it = histo[i].begin(); it!=histo[i].end() ; it++ ){
            rectangle(histogram_image, Point((HISTO_SIZE*i) + 2*it->first, histogram_image.rows - (it->second >> ractangle_divider)), 
                Point((HISTO_SIZE*i) + 2*(it->first+1), histogram_image.rows),i <3 ? Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0):Scalar(255,255,255));
        }
        const string toprint = to_string(entropy[i]);
        putText(histogram_image, toprint , Point(250 + (HISTO_SIZE*i) , 200), FONT_HERSHEY_COMPLEX_SMALL ,0.8,i <3 ? Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0):Scalar(255,255,255) );
        putText(histogram_image, "entropy : " , Point(150 + (HISTO_SIZE*i) , 200), FONT_HERSHEY_COMPLEX_SMALL ,0.8,i <3 ? Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0):Scalar(255,255,255) );
    }
    return histogram_image;
}
Mat snrOnHisto(Mat histo,vector<double> snr ){
    for (int i = 0; i < COLOR_CHANNELS; i++){
        const string toprint = to_string(snr[i]);
        putText(histo, toprint , Point(250 + (HISTO_SIZE*i) , 300), FONT_HERSHEY_COMPLEX_SMALL ,0.8,i <3 ? Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0):Scalar(255,255,255) );
        putText(histo, "snr : " , Point(175 + (HISTO_SIZE*i) , 300), FONT_HERSHEY_COMPLEX_SMALL ,0.8,i <3 ? Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0):Scalar(255,255,255) );
    }
    return histo;
}
vector<double> histoEntropy(vector<map<short,int>> histo, int sample_size ){
    vector<double> entropy;
    for(int i = 0; i < histo.size(); i++){
        double tmp = 0;
        entropy.push_back(tmp);
        for(it = histo[i].begin(); it!=histo[i].end() ; it++ ){
            double prob = (double)it->second/((double)sample_size*HISTO_BITSCAPTURE);
            entropy[i] = entropy[i] - prob*log(prob);
        }
    }
    return entropy;
}
Mat compress(Mat image, int color = IMG_COLOR ,uint8_t bits=SHIFT_BITS){
    Mat image_compressed(image.rows,image.cols,CV_8UC3,Scalar(0,0,0));
    // if(color != 0){
        for (int r = 0; r < image.rows; r++){
            for (int c = 0; c < image.cols; c++){
                for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
                    image_compressed.at<Vec3b>(r,c)[channel] = image.at<Vec3b>(r,c)[channel]>>bits;            
                }
            }
        }
    // }else{
    //     for (int r = 0; r < image.rows; r++){
    //         for (int c = r%HISTO_BITSCAPTURE; c < image.cols; c=c+HISTO_BITSCAPTURE){
    //             image_compressed.at<uint32_t>(r,c) = image.at<uint32_t>(r,c)>> bits;                            
    //         }
    //     }
    // }
    return image_compressed;
}
Mat decompress(Mat image,int color = IMG_COLOR, uint8_t bits=SHIFT_BITS){
    Mat image_decompressed(image.rows,image.cols,CV_8UC3,Scalar(0,0,0));
    // if(color != 0){
        for (int r = 0; r < image.rows; r++){
            for (int c = r%HISTO_BITSCAPTURE; c < image.cols; c=c+HISTO_BITSCAPTURE){
                for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
                    image_decompressed.at<Vec3b>(r,c)[channel] = image.at<Vec3b>(r,c)[channel]<<bits;            
                }
            }
        }
    // }else{
    //     for (int r = 0; r < image.rows; r++){
    //         for (int c = r%HISTO_BITSCAPTURE; c < image.cols; c=c+HISTO_BITSCAPTURE){
    //             image_decompressed.at<uint32_t>(r,c) = image.at<uint32_t>(r,c) << bits;            
    //         }
    //     }
    // }
    return image_decompressed;
}
vector<double> signalToNoise(Mat image1, Mat image2){
    vector<double> sums;
    vector<double> psnr;
    for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
        double sum = 0;
        sums.push_back(sum);
        for (int r = 0; r < image1.rows; r++){
            for (int c = 0; c < image2.cols; c++){
                sums[channel] = sums[channel] + pow(image1.at<Vec3b>(r,c)[channel] - image2.at<Vec3b>(r,c)[channel],2);
                
            }
        }
        double snr = 10*log10(pow(256,2)*image1.rows*image1.cols/sums[channel]);
        psnr.push_back(snr);
    }
    return psnr;
}
void saveImage(string path,Mat image, uint8_t show = 1 ){
    if(show){
        namedWindow(path, WINDOW_AUTOSIZE );
        imshow(path, image);
    }
    imwrite(path, image);
}
int main(int argc, char** argv ) // main para usar imagens (meter WAIT_KEY a 0)
{
    Mat image = imread(argc!=2 ?"../Images/cao.jpg":argv[1],IMG_COLOR);
    Mat image_compressed = compress(image);
    Mat image_decompressed = decompress(image_compressed);
    vector<double> snr = signalToNoise(image, image_decompressed);
    
    vector<map<short,int>> histo = createHistogram(image);
    string file = "../Histograms/histo.txt";
    histoToFile(file,histo);
    vector<double> entropy = histoEntropy(histo,image.rows*image.cols/HISTO_BITSCAPTURE);
    Mat histo_image = imageHisto(histo,entropy);

    
    vector<map<short,int>> histoOut = createHistogram(image_decompressed);
    histoToFile(file,histo);
    vector<double> entropyOut = histoEntropy(histoOut,image_decompressed.rows*image_decompressed.cols/HISTO_BITSCAPTURE);
    Mat histo_image_decompressed = snrOnHisto(imageHisto(histoOut,entropyOut,5),snr);
    
    if ( !image.data )
    {
        printf("No image data \n");
        return -1; 
    }
    // saveImage("imagem.jpg", image);
    saveImage(argc!=3 ?"../Images_Out/out.jpg":argv[2], image_decompressed);
    saveImage("../Histograms/histo.jpg", histo_image);
    saveImage("../Histograms/histo_out.jpg", histo_image_decompressed);
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
