#include <iostream>
#include <fstream>
#include "../../AudioFile-master/AudioFile.h"
using namespace std;

int main(int argc, char* argv[]) {

    string fileToCopy = string(argv[1]) + ".txt";
    string newFileName = string(argv[2]) + ".txt";

    cout << "File to copy:  " << fileToCopy << endl;
    cout << "New file name: " << newFileName << endl;
    //cout << "Content: " << endl;

    ifstream ifs(fileToCopy);
    ofstream ofs(newFileName);

    char c;
    while(true) {
        ifs.get(c);
        if (ifs.eof()) break;
        ofs.put(c);
        //cout << c << endl;
    }
    ofs.close();
    
    return 0;
}
