//------------------------------------------------------------------------------
// File:   streaming.cpp
// Author: ¿?
// Date:   ¿?
// Coms:   Código principal del servicio de streaming
//         Emplea el fichero resultante de ejecutar el script filtro
//         Se comunica con el programa masterWorker
// Use:    Para usarlo se debe invocar de la siguiente manera
//         ./streaming XXXX
//         siendo XXXX el número del puerto a emplear
//------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include "Socket.hpp"
using namespace std;
const string F_INPUT = "tweets-procesados.csv";

int main(int argc, char* argv[]) {
    if(argc == 2) {
        int PORT = stoi(argv[1]);
        cout << PORT << endl;
    }
    else {
        cout << "Ejecutar de la siguiente forma:" << endl;
        cout << "./streaming XXXX" << endl;
        cout << "siendo XXXX el número del puerto a emplear" << endl;
    }
    return 0;
}