//------------------------------------------------------------------------------
// File:   analizadorTags.cpp
// Authors: Axel Isaac Pazmiño Ortega y Alicia Lázaro Huerta
// Date:   23/12/2021
// Coms:   Código principal del programa analizadorTags
//         Se comunica con gestorDeColas
// Use:    Para usarlo se debe invocar de la siguiente manera
//         ./analizadorTags XXXX YYYYYYYYY
//         siendo XXXX el número del puerto a emplear para comunicarse con gestor de colas
//         siendo YYYYYYYYY la ip a emplear para comunicarse con gestor de colas
//------------------------------------------------------------------------------
#include "Socket/Socket.hpp"
#include <thread>
#include <ctime>
#include "Semaphore_V4/Semaphore_V4.hpp"
using namespace std;

int main(int argc, char* argv[]) {
    if(argc == 3) {
        //VARIABLES DE INVOCACIÓN
        int PORT_GESTOR = stoi(argv[1]);
        string IP_GESTOR = string(argv[2]);
        
        string MENS_FIN = "FIN";
        // Creación del socket con el que se llevará a cabo
        // la comunicación con el servidor gestor.
        Socket chanGestor(IP_GESTOR, PORT_GESTOR);

        // Conectamos con el servidor. Probamos varias conexiones
        const int MAX_ATTEMPS = 10;
        int count = 0;
        int socket_fd_gestor;
        do {
            // Conexión con el servidor
            socket_fd_gestor = chanGestor.Connect();
            count++;

            // Si error --> esperamos 1 segundo para reconectar
            if(socket_fd_gestor == -1) {
                this_thread::sleep_for(chrono::seconds(1));
            }
        } while(socket_fd_gestor == -1 && count < MAX_ATTEMPS);

        // Chequeamos si se ha realizado la conexión
        if(socket_fd_gestor == -1) {
            
        }
        cout << "CONEXION ESTABLECIDA" << endl;

        // declaración de variables para intercambio de mensajes:
        const string MESSAGE = "READ_TAGS";
        string respuesta;
        int send_bytes,read_bytes;
        int LENGTH = 10000;
        // Enviamos el mensaje de petición al servicio gestor
        send_bytes = chanGestor.Send(socket_fd_gestor, MESSAGE);
            
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            chanGestor.Close(socket_fd_gestor);
            exit(1);
        }

        bool out = false;
        while (!out) {
            // Recibimos la respuesta del servidor de streaming
            read_bytes = chanGestor.Recv(socket_fd_gestor, respuesta, LENGTH);
        
            if(read_bytes == -1) {
                cerr << "Error al recibir datos: " << strerror(errno) << endl;
                // Cerramos el socket
                chanGestor.Close(socket_fd_gestor);
                exit(1);
            } 
            
            if (respuesta == MENS_FIN) {
                out = true;
            }

            else {
                // tratamiento de la información recibida
                // cout << respuesta + "\n";   // solo para pruebas

            }
            if(respuesta != MENS_FIN) {
                // Enviamos el mensaje de petición al servicio gestor
                send_bytes = chanGestor.Send(socket_fd_gestor, MESSAGE);
                    
                if(send_bytes == -1) {
                    cerr << "Error al enviar datos: " << strerror(errno) << endl;
                    // Cerramos el socket
                    chanGestor.Close(socket_fd_gestor);
                    exit(1);
                }
            }
        }
        

        cout << "BYE BYE" << endl;
     }
    else {
        cout << "Ejecutar de la siguiente forma:" << endl;
        cout << "./masterWorker XXXX YYYYYYYYY ZZZZ WWWWWWWWW" << endl;
        cout << "siendo XXXX el número del puerto a emplear para comunicarse con streaming" << endl;
        cout << "siendo YYYYYYYYY la ip a emplear para comunicarse con streaming" << endl;
        cout << "siendo ZZZZ el número del puerto a emplear para comunicarse con gestor de colas" << endl;
        cout << "siendo WWWWWWWWW la ip a emplear para comunicarse con gestor de colas" << endl;
    }
}