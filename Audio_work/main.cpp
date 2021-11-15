#include <fstream>
#include <iostream>
#include <system_error>
#include "AudioFile/AudioFile.h"
#include <math.h> // somewhere earler (for M_PI and sinf())
#include <map>
#include <opencv2/opencv.hpp>
// using namespace wave;
#define DECIMAL_SPACES 1000
#define WAIT_KEY 0          //0 para imagem e 1 para video
#define IMG_COLOR -1
#define HISTO_WINDOWSIZE_Y 1000
#define HISTO_WINDOWSIZE_X 1500
#define HISTO_SIZE 530
#define HISTO_NUMCOLLUMS 1 //quanto maior for o numero, menos colunas aparecem
using namespace std;
using namespace cv;
vector<map<double,int>> histo;
map<double,int>::iterator it;
Mat imageHisto(vector<double> entropy){
    Mat histogram_image(HISTO_WINDOWSIZE_Y, HISTO_WINDOWSIZE_X,CV_8UC3, Scalar(0,0,0));

    for (int i = 0; i < histo.size(); i++){
        for(it = histo[i].begin(); it!=histo[i].end() ; it++ ){
            rectangle(histogram_image, Point((HISTO_SIZE*i) +  2*((it->first+1)*250) , histogram_image.rows - (it->second >> 4)), 
                Point((HISTO_SIZE*i) + 2*((it->first+1)*250+1), histogram_image.rows),Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0));
        }
        const string toprint = to_string(entropy[i]);
        putText(histogram_image, toprint , Point(250 + (HISTO_SIZE*i) , 200), FONT_HERSHEY_COMPLEX_SMALL ,0.8, Scalar(i == 0?255:0 ,i == 1?255:0,i == 2?255:0));
    }
    return histogram_image;
}
vector<double> histoEntropy(int numChannels, int numSamples ){
    vector<double> entropy;
    for(int i=0 ; i < numChannels; i++){
        double tmp = 0;
        entropy.push_back(tmp);
    }
    for(int i = 0; i < histo.size(); i++){
        for(it = histo[i].begin(); it!=histo[i].end() ; it++ ){
            double prob = (double)it->second/(double)numSamples;
            entropy[i] = entropy[i] - prob*((prob ? log(prob) : 0)/log(histo[i].size()));
        }
    }
    return entropy;
}
AudioFile<double>::AudioBuffer fillbuffer(AudioFile<double> audioFile){
  // Create an AudioBuffer 
  AudioFile<double>::AudioBuffer buffer;
  int numSamples = audioFile.getNumSamplesPerChannel();
  int numChannels = audioFile.getNumChannels();

  buffer.resize(numChannels);

  // Set number of samples per channel,i
  for(int i=0 ; i <numChannels; i++){
    map<double,int> mapa;
    histo.push_back(mapa);
    buffer[i].resize(numSamples);

  }

  // fill the buffer with samples, e.g.
  double tmp;
  for (int i = 0; i < numSamples; i++)
  {
    for (int channel = 0; channel < numChannels; channel++){
        tmp = audioFile.samples[channel][i];
        int a = tmp*DECIMAL_SPACES;
        double temp = (double)a/DECIMAL_SPACES;
        histo[channel].find(temp); 
        if(it == histo[channel].end()) histo[channel][temp]=0;
        histo[channel][temp]++;
        buffer[channel][i] = temp;
    }
  }
  return buffer;
}
void histoToFile(int numChannels){
    ofstream ofs("../histo.txt");
    for(int i = 0; i< numChannels; i++){
        ofs << "canal numero: " << i << endl;
        for(it = histo[i].begin(); it!=histo[i].end() ; it++ ){
            ofs<< it->first << " : " << it->second << endl;

        }
    }
}
int main(int argc, char** argv) {
    const char* fileIn = argc > 1? argv[1]:"../../Wav files-20211025/sample01.wav";
    const char* fileOut = argc > 2? argv[2]:"../out.wav";
    
    AudioFile<double> audioFile;
    audioFile.load(fileIn);


    // Create AudioFileOut
    AudioFile<double> audioFileOut;
    AudioFile<double>::AudioBuffer buffer = fillbuffer(audioFile);
    bool ok = audioFileOut.setAudioBuffer(buffer);
    cout << "lol" << endl;

    audioFileOut.setBitDepth(audioFile.getBitDepth());
    audioFileOut.setNumChannels(audioFile.getNumChannels());
    audioFileOut.setNumSamplesPerChannel(audioFile.getNumSamplesPerChannel());
    audioFileOut.setSampleRate(audioFile.getSampleRate());

    // or, just use this quick shortcut to print a summary to the console
    if(ok) audioFileOut.save(fileOut);
    int numChannels = audioFile.getNumChannels();
    histoToFile(numChannels);
    printf("ficheiro de entrada : %s \n", fileIn);
    audioFile.printSummary();
    printf("ficheiro de saida : %s \n", fileOut);
    audioFileOut.printSummary();
    cout << "lol2" << endl;
    vector<double> entropy = histoEntropy(numChannels,audioFile.getNumSamplesPerChannel());
    Mat histo_image = imageHisto(entropy);
    cout << "lol3" << endl;
    namedWindow("Histogram", WINDOW_AUTOSIZE );
    imshow("Histogram", histo_image);
    imwrite("../histo.jpg", histo_image);
    waitKey(WAIT_KEY);
    return 0;
}

