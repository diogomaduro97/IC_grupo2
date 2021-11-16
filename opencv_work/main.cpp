#include <stdio.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <map>
#include <math.h>
using namespace cv;
using namespace std;
#define WAIT_KEY 0          //0 para imagem e 1 para video
#define COLOR_CHANNELS 4
#define IMG_COLOR 0
#define HISTO_BITSCAPTURE 1 // quanto maior o numero menos pixeis sao contados para o hitograma, mais rapida e a captura de ecra
#define HISTO_WINDOWSIZE_Y 1000
#define HISTO_WINDOWSIZE_X 2300
#define HISTO_SIZE 510
#define RECTANGLE_SIZE 2 // quanto maior o numero menor o tamanho do rectagulo
#define SHIFT_BITS 2
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
                Point((HISTO_SIZE*i) + 2*(it->first+1), histogram_image.rows),i <3 ? Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0):Scalar(255,255,255));
        }
        const string toprint = to_string(entropy[i]);
        putText(histogram_image, toprint , Point(250 + (HISTO_SIZE*i) , 200), FONT_HERSHEY_COMPLEX_SMALL ,0.8,i <3 ? Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0):Scalar(255,255,255) );
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
void saveImage(string path,Mat image){
    namedWindow(path, WINDOW_AUTOSIZE );
    imshow(path, image);
    imwrite(path, image);
}
Mat compress(Mat image, int color = IMG_COLOR ,uint8_t bits=SHIFT_BITS){
    Mat image_compressed(image.rows,image.cols,CV_8UC3,Scalar(0,0,0));
    // if(color != 0){
        for (int r = 0; r < image.rows; r++){
            for (int c = 0; c < image.cols; c++){
                for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
                    image_compressed.at<Vec4b>(r,c)[channel] = image.at<Vec4b>(r,c)[channel]>>bits;            
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
                    image_decompressed.at<Vec4b>(r,c)[channel] = image.at<Vec4b>(r,c)[channel] << bits;            
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
    for(int i=0 ; i < COLOR_CHANNELS; i++){
        double sum = 0;
        sums.push_back(sum);
    }
    for (int r = 0; r < image1.rows; r++){
        for (int c = 0; c < image2.cols; c++){
            for (size_t channel = 0; channel < COLOR_CHANNELS; channel++){
                sums[channel] = sums[channel] + pow(image1.at<Vec4b>(r,c)[channel] - image2.at<Vec4b>(r,c)[channel],2);
                
            }
        }
    }
    
    vector<double> psnr;
    for(int i=0 ; i < COLOR_CHANNELS; i++){
        double snr = 10*log10(pow(256,2)*image1.rows*image1.cols/sums[i]);
        psnr.push_back(snr);
    }
    return psnr;
}
int main(int argc, char** argv ) // main para usar imagens (meter WAIT_KEY a 0)
{
    /* 
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    } */
    Mat image = imread("gaja.jpg");
    Mat imagemout = compress(image);
    Mat imagemout2 = decompress(imagemout);
    string file = "histo.txt";
    vector<double> snr = signalToNoise(image, imagemout2);
    for(int i = 0; i < snr.size(); i++){
        cout << snr[i] << endl;
    }
    
    vector<map<short,int>> histo = createHistogram(image);
    histoToFile(file,histo);
    vector<double> entropy = histoEntropy(histo,image.rows*image.cols/HISTO_BITSCAPTURE);

    Mat histo_image = imageHisto(histo,entropy);
    if ( !image.data )
    {
        printf("No image data \n");
        return -1; 
    }
    saveImage("imagem.jpg", image);
    saveImage("imagemou1.jpg", imagemout);
    saveImage("imagemout2.jpg", imagemout2);
    saveImage("histo.jpg", histo_image);

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
