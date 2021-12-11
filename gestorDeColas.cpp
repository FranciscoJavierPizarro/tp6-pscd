//------------------------------------------------------------------------------
// File:   gestorDeColas.cpp
// Author: ¿?
// Date:   ¿?
// Coms:   Código principal del gestor de colas, este emplea monitores para
//         gestionar los problemas de concurrencia y el tad genérico
//         BoundedQueue para implementar las colas
//         Se comunica con los programas masterWorker, analizadorTags y
//         analizadorRendimiento
// Use:    Para usarlo se debe invocar de la siguiente manera
//         ./gestorDeColas XXXX ZZZZ
//         siendo XXXX el número del puerto a emplear para comunicarse con masterWorker
//         siendo ZZZZ el número del puerto a emplear para comunicarse con los analizadores
//------------------------------------------------------------------------------
#include <iostream>
#include "Socket.hpp"
#include "monitor.hpp"
#include "BoundedQueue.hpp"
using namespace std;

int main(int argc, char* argv[]) {
    if(argc == 3) {
        int PORT_MASTERWORKER = stoi(argv[1]);
        cout << PORT_MASTERWORKER << endl;
        int PORT_ANALIZADORES = stoi(argv[2]);
        cout << PORT_ANALIZADORES << endl;
    }
    else {
        cout << "Ejecutar de la siguiente forma:" << endl;
        cout << "./gestorDeColas XXXX ZZZZ" << endl;
        cout << "siendo XXXX el número del puerto a emplear para comunicarse con masterWorker" << endl;
        cout << "siendo ZZZZ el número del puerto a emplear para comunicarse con los analizadores" << endl;
    }
    return 0;
}