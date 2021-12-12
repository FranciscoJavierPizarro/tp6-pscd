//------------------------------------------------------------------------------
// File:   masterWorker.cpp
// Author: Inés Román Gracia
//         Francisco Javier Pizarro Martínez
// Date:   11/12/2021
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
#include <thread>
#include <ctime>
using namespace std;
/*
Current state:
Protocolo de comunicaciones hecho
Falta de implementar el procesamiento de las respuestas y 
concretar/implementar que deben enviar los workers


*/
const int N_WORKERS = 5;

void master(int PORT_STREAMING, string IP_STREAMING, int PORT_GESTOR, string IP_GESTOR) {
    string MENS_FIN = "FIN";
    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
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
    Socket chanStream(IP_GESTOR, PORT_GESTOR);

    // Conectamos con el servidor. Probamos varias conexiones
    count = 0;
    int socket_fd_streaming;
    do {
        // Conexión con el servidor
        socket_fd_streaming = chanStream.Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if(socket_fd_streaming == -1) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } while(socket_fd_streaming == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if(socket_fd_streaming == -1) {
        
    }
    int LENGTH = 10000;
    string tareas[5];
    string mensaje;
    int read_bytes;   //num de bytes recibidos en un mensaje
    int send_bytes;  //num de bytes enviados en un mensaje

    mensaje = "GET_TWEETS";
    
    for(int j = 0; j < 1; j++) {
        // Enviamos el mensaje de petición al servicio de streaming
        send_bytes = chanStream.Send(socket_fd_streaming, mensaje);
            
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            chanStream.Close(socket_fd_streaming);
            exit(1);
        }

        // Recibimos la respuesta del servidor de streaming
        read_bytes = chanStream.Recv(socket_fd_streaming, mensaje, LENGTH);
        
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            chanStream.Close(socket_fd_streaming);
            exit(1);
        }    

        //PROCESAR RESPUESTA



    //¿????¿

        for(int i = 0; i < 5; i++) {
            // Enviamos el bloque de tareas al gestor de colas
            send_bytes = chanGestor.Send(socket_fd_gestor, tareas[i]);
                
            if(send_bytes == -1) {
                cerr << "Error al enviar datos: " << strerror(errno) << endl;
                // Cerramos el socket
                chanGestor.Close(socket_fd_gestor);
                exit(1);
            }
        }
    }
    mensaje = MENS_FIN;
    // Enviamos el mensaje de fin al servicio de streaming
    send_bytes = chanStream.Send(socket_fd_streaming, mensaje);
            
    if(send_bytes == -1) {
        cerr << "Error al enviar datos: " << strerror(errno) << endl;
        // Cerramos el socket
        chanStream.Close(socket_fd_streaming);
        exit(1);
    }
    // Enviamos el mensaje de fin al servicio gestor de colas
    send_bytes = chanGestor.Send(socket_fd_gestor, mensaje);
            
    if(send_bytes == -1) {
        cerr << "Error al enviar datos: " << strerror(errno) << endl;
        // Cerramos el socket
        chanGestor.Close(socket_fd_gestor);
        exit(1);
    }



    // Cerramos el socket
    int error_code = chanStream.Close(socket_fd_streaming);
    if(error_code == -1) {
        cerr << "Error cerrando el socket: " << strerror(errno) << endl;
    }

    // Cerramos el socket
    int error_code = chanGestor.Close(socket_fd_gestor);
    if(error_code == -1) {
        cerr << "Error cerrando el socket: " << strerror(errno) << endl;
    }
}

void worker(int PORT_GESTOR, string IP_GESTOR, int id) {
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

    int LENGTH = 10000;
    string tweets[5];
    string mensaje = "";
    int read_bytes;   //num de bytes recibidos en un mensaje
    int send_bytes;  //num de bytes enviados en un mensaje
    bool out = false;
    while(!out) {
        // Enviamos el mensaje de petición al servicio gestor
        mensaje = "READ_TAREAS," + to_string(id);
        send_bytes = chanGestor.Send(socket_fd_gestor, mensaje);
            
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            chanGestor.Close(socket_fd_gestor);
            exit(1);
        }

        // Recibimos la respuesta del servidor gestor
        read_bytes = chanGestor.Recv(socket_fd_gestor, mensaje, LENGTH);
        
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            chanGestor.Close(socket_fd_gestor);
            exit(1);
        }

        if(mensaje == MENS_FIN) { 
            out = true;
        }
        else{
            //PROCESAR

            //¿=¿??¿?¿?¿
            // Enviamos el mensaje de petición al gestor
            mensaje = "PUBLISH_QOS,datos????," + to_string(id);
            send_bytes = chanGestor.Send(socket_fd_gestor, mensaje);
                
            if(send_bytes == -1) {
                cerr << "Error al enviar datos: " << strerror(errno) << endl;
                // Cerramos el socket
                chanGestor.Close(socket_fd_gestor);
                exit(1);
            }
            // Enviamos el mensaje de petición al gestor
            mensaje = "PUBLISH_TAGS,datos?????," + to_string(id);
            send_bytes = chanGestor.Send(socket_fd_gestor, mensaje);
                
            if(send_bytes == -1) {
                cerr << "Error al enviar datos: " << strerror(errno) << endl;
                // Cerramos el socket
                chanGestor.Close(socket_fd_gestor);
                exit(1);
            }
        }




    }

    // Cerramos el socket
    int error_code = chanGestor.Close(socket_fd_gestor);
    if(error_code == -1) {
        cerr << "Error cerrando el socket: " << strerror(errno) << endl;
    }
}

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