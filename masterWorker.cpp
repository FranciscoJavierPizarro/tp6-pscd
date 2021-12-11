//------------------------------------------------------------------------------
// File:   masterWorker.cpp
// Author: ¿?
// Date:   ¿?
// Coms:   Código principal del programa masterWorker
//         Se comunica con gestorDeColas y con streaming
// Use:    Para usarlo se debe invocar de la siguiente manera
//         ./masterWorker XXXX YYYYYYYYY ZZZZ WWWWWWWWW
//         siendo XXXX el número del puerto a emplear para comunicarse con streaming
//         siendo YYYYYYYYY la ip a emplear para comunicarse con streaming
//         siendo ZZZZ el número del puerto a emplear para comunicarse con gestor de colas
//         siendo WWWWWWWWW la ip a emplear para comunicarse con gestor de colas
//------------------------------------------------------------------------------
#include <iostream>
#include "Socket/Socket.hpp"
using namespace std;

int main(int argc, char* argv[]) {
    if(argc == 5) {
        int PORT_STREAMING = stoi(argv[1]);
        cout << PORT_STREAMING << endl;
        string IP_STREAMING = string(argv[2]);
        cout << IP_STREAMING << endl;
        int PORT_GESTOR = stoi(argv[3]);
        cout << PORT_GESTOR << endl;
        string IP_GESTOR = string(argv[4]);
        cout << IP_GESTOR << endl;
    }
    else {
        cout << "Ejecutar de la siguiente forma:" << endl;
        cout << "./masterWorker XXXX YYYYYYYYY ZZZZ WWWWWWWWW" << endl;
        cout << "siendo XXXX el número del puerto a emplear para comunicarse con streaming" << endl;
        cout << "siendo YYYYYYYYY la ip a emplear para comunicarse con streaming" << endl;
        cout << "siendo ZZZZ el número del puerto a emplear para comunicarse con gestor de colas" << endl;
        cout << "siendo WWWWWWWWW la ip a emplear para comunicarse con gestor de colas" << endl;
    }
    return 0;
}