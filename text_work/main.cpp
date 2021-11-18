#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
#include <opencv2/opencv.hpp>
#include <math.h>
using namespace cv;
using namespace std;
#define HISTO_WINDOWSIZE_X 1000
#define HISTO_WINDOWSIZE_Y 700
// template <class T>
// void func(T x){
//     cout << "size = " << sizeof(x) << "  " << x << endl;
// }

map<char,int>::iterator it;

int max(map<char,int> histo) {
  int max = 0;
  for (it = histo.begin(); it != histo.end(); it++) {
    if (it->second > max) max = it->second;
  }
  return max;
}

double histoToEntropy(map<char,int> histo, int total) {
  double entropy;
  for (it = histo.begin(); it != histo.end(); it++) {
    double prob = (double)(it->second) / (double)(total);
    entropy -= prob*log2(prob);
  }
  return entropy;
}

Mat imageHisto(map<char,int> histo, double entropy, int total) {
  int divider = max(histo);
  int bar_size = HISTO_WINDOWSIZE_X / 52;
  Mat histogram_image(HISTO_WINDOWSIZE_Y, HISTO_WINDOWSIZE_X, CV_8UC3, Scalar(0,0,0));
  int x = 0;
  for (it = histo.begin(); it != histo.end(); it++) {
    string letter = "";
    char l = it->first;
    letter += l;
    rectangle(histogram_image,
      Point(x, HISTO_WINDOWSIZE_Y-50-(it->second*(HISTO_WINDOWSIZE_Y-100)/divider)),
      Point(x+bar_size, HISTO_WINDOWSIZE_Y-50),
      Scalar(255,255,255));
    putText(histogram_image, letter, Point(x+bar_size/2-3,HISTO_WINDOWSIZE_Y-20), FONT_HERSHEY_COMPLEX_SMALL, 0.6, Scalar(255,255,255));
    x += bar_size;
  }
  putText(histogram_image, "Entropy = "+to_string(entropy), Point(HISTO_WINDOWSIZE_X/8,200), FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(255,255,255));
  putText(histogram_image, "Total letters = "+to_string(total), Point(HISTO_WINDOWSIZE_X/8,220), FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(255,255,255));
  return histogram_image;
}

int main(int argc, char** argv){                        //copiar ficheiro character a character
    //usar a mesma funçao com inputs diferentes
    // func(5.5);
    // func(1000); 
    // func('x'); 
    // func(static_cast<char>(64)); 
    // ler ficheiros
    map<char,int> histo;
    map<char,int>::iterator it;
    ifstream ifs(argc > 1? argv[1]:"../english_book.txt");
    ofstream ofs(argc > 2? argv[2]:"../out.txt");
    string line;
    int count = 0;
    while(getline(ifs,line, '\0')){
        for(char x : line){
          if(isalpha(x)) {
            it = histo.find(x);
            if(it == histo.end()) histo[x]=0;
            histo[x]++;
            // cout << x;
            count++;
        }
        ofs << x;
      }
    }
    cout<<endl;
    for(it = histo.begin(); it!=histo.end() ; it++ ){
      cout<< it->first << " : " << it->second << endl;
    }
    double entropy = histoToEntropy(histo,count);
    Mat histo_image = imageHisto(histo,entropy,count);
    string path = "../histogram.jpg";
    namedWindow(path, WINDOW_AUTOSIZE);
    imshow(path,histo_image);
    imwrite(path,histo_image);
    waitKey(0);
    /* for(int i = 0; i < 5; i++)
    {
        cout << "ola" << endl;
    } */
    // int a[5] = {1,2,3,4,5};
    // for(int i : a){
    //     cout << i << endl;
    // }
    // ler um ficheiro e guardar num vector

    // ler um ficheiro e calcular o numero de instancias em que ocorre aquela letra

    return 0;  
}
