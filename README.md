# IC_grupo2

## Description

	-Simple C++ programs to analize audio and image
	-Generate histograms and the given entropy on audio and images for each channel or color channel
	-Uses a simple uniform quantization method
	-Calculates the Peak Signal to Noise Ratio, between two images or audios
	-Audio tested with WAV files
	-Images tested with jpg
	-Needs Opencv and Adam Stark's AudioFile library installed, the later needs to be installed in the Audio_work directory

## How to run
### text_work:
		$ cd text_work
		$ mkdir build && cd build
		$ cmake ../
		$ make
		$ ./test [arg1][arg2]

### Audio_work:
  
	-Instalation of the Adam Stark's library:
		
		$ cd Audio_work    
		$ git clone git@github.com:adamstark/AudioFile.git
		$ cd AudioFile
		$ mkdir build && cd build
		$ cmake ../
		$ make

  
	-To run Audio_work:
    
		$ cd Audio_work
		$ mkdir build && cd build
		$ cmake ../
		$ make
		$ ./audioTest [arg1][arg2]
### opencv_work:
    
		$ cd opencv_work
		$ mkdir build && cd build
		$ cmake ../
		$ make
		$ ./opencvTest [arg1][arg2]
## Reference
	https://github.com/adamstark/AudioFile
	https://github.com/opencv/opencv
