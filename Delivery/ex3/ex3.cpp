#include <fstream>
#include <iostream>
#include <system_error>
//#include "AudioFile/AudioFile.h"
#include "../../AudioFile-master/AudioFile.h"
#include <math.h> // somewhere earler (for M_PI and sinf())
#include <map>
// using namespace wave;
using namespace std;

AudioFile<double>::AudioBuffer fillbuffer(AudioFile<double> audioFile){
  // Create an AudioBuffer 
  AudioFile<double>::AudioBuffer buffer;
  int numSamples = audioFile.getNumSamplesPerChannel();
  int numChannels = audioFile.getNumChannels();

  buffer.resize(numChannels);

  // Set number of samples per channel,i
  for(int i = 0 ; i < numChannels; i++) {
    buffer[i].resize(numSamples);
  }

  // fill the buffer with samples, e.g.
  double temp;
  for (int i = 0; i < numSamples; i++)
  {
    for (int channel = 0; channel < numChannels; channel++) {
        temp = audioFile.samples[channel][i];
        buffer[channel][i] = temp;
    }
  }
  return buffer;
}

int main(int argc, char** argv) {
    const char* fileIn = argc > 1? argv[1]:"8k8bitpcm.wav";
    const char* fileOut = argc > 2? argv[2]:"out.wav";
    AudioFile<double> audioFile;
    audioFile.load(fileIn);


    // Create AudioFileOut
    AudioFile<double> audioFileOut;
    AudioFile<double>::AudioBuffer buffer = fillbuffer(audioFile);
    bool ok = audioFileOut.setAudioBuffer( buffer);

    audioFileOut.setBitDepth(audioFile.getBitDepth());
    audioFileOut.setNumChannels(audioFile.getNumChannels());
    audioFileOut.setNumSamplesPerChannel(audioFile.getNumSamplesPerChannel());
    audioFileOut.setSampleRate(audioFile.getSampleRate());

    //quick shortcut to print a summary to the console
    if(ok) audioFileOut.save(fileOut);
    int numChannels = audioFile.getNumChannels();
    printf("Ficheiro de entrada : %s \n", fileIn);
    audioFile.printSummary();
    printf("Ficheiro de saida : %s \n", fileOut);
    audioFileOut.printSummary(); 
    return 0;
}

