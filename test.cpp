#include <iostream>
#include <fstream>
#include <map>
using namespace std;
template <class T>
void func(T x){
    cout << "size = " << sizeof(x) << "  " << x << endl;
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
    ifstream ifs(argc > 1? argv[1]:"nums.txt");
    ofstream ofs(argc > 2? argv[2]:"out.txt");
    string line;
    while(getline(ifs,line, '\0')){
        for(char x : line){
          it = histo.find(x);
          if(it == histo.end()) histo[x]=0;
          histo[x]++;
          cout << x;
          ofs << x;
        }
    }
    cout<<endl;
    for(it = histo.begin(); it!=histo.end() ; it++ ){
      cout<< it->first << " : " << it->second << endl;
    }
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

