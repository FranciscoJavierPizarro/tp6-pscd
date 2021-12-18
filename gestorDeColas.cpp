//------------------------------------------------------------------------------
// File:   gestorDeColas.cpp
// Author: Pablo López Mosqueda
//         Leonor Murphy Anía
// Date:   18/12/2021
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
#include <thread>
#include "Socket/Socket.hpp"
#include "Monitor/monitor.hpp"
#include "BoundedQueue/BoundedQueue.hpp"
using namespace std;

void tareas(Socket socTareas, int fd, controlDeCola controlTareas){
    BoundedQueue<string> colaTareas;
    string elemento;
    int length = 10000;
    char buffer[length];
    char * mensaje;
    char * datos;
    bool out = false;

    while(!out){
        int rcv_bytes = socTareas.Recv(fd,buffer,length);
        if (rcv_bytes == -1) {
            cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
            // Cerramos los sockets
            socTareas.Close(fd);
        }
        mensaje = strtok (buffer," ,");
        datos = strtok (NULL, " ,");
        
        if(mensaje == "FIN"){
            out = true;
        }
        else if(mensaje == "READ_TAREAS"){
            controlTareas.leerCola(colaTareas,elemento);
            int send_bytes = socTareas.Send(fd, elemento); //enviar el elemento leído
            if(send_bytes == -1) {
                string mensError(strerror(errno));
                cerr << "Error al enviar datos: " + mensError + "\n";
                // Cerramos los sockets
                socTareas.Close(fd);
                exit(1);
            }
        }
        else if(mensaje == "PUBLISH_TAREAS"){
            controlTareas.escribirCola(colaTareas, datos);
        }
    }
    socTareas.Close(fd); //cerrar el socket
}

void tags(){

}

void QoS(){

}

int main(int argc, char* argv[]) {
    if(argc == 3) {
        int PORT_MASTERWORKER = stoi(argv[1]);
        cout << PORT_MASTERWORKER << endl;
        int PORT_ANALIZADORES = stoi(argv[2]);
        cout << PORT_ANALIZADORES << endl;

        const int N_COLAS = 3;
        
        //3 monitores, uno para cada cola
        ControldeCola monitores[N_COLAS]; 
         
        thread cliente[N_COLAS];
        int client_fd[N_COLAS];

        // Creación del socket con el que se llevará a cabo la comunicación con el servidor.
        Socket socTareas(PORT_MASTERWORKER);
        //FALTA EL SOCKET DE LAS OTRAS DOS COLAS

        // Bind
        int socket_fd = socTareas.Bind();
        if (socket_fd == -1) {
            cerr << "Error en el bind: " + string(strerror(errno)) + "\n";
            exit(1);
        }

        // Listen
        int max_connections = 2;
        int error_code = socTareas.Listen(max_connections);
        if (error_code == -1) {
            cerr << "Error en el listen: " + string(strerror(errno)) + "\n";
            // Cerramos el socket
            socTareas.Close(socket_fd);
            exit(1);
        }
        // Accept
        client_fd[0] = socTareas.Accept();
        if(client_fd[0] == -1) {
                cerr << "Error en el accept: " + string(strerror(errno)) + "\n";
                // Cerramos el socket
                socTareas.Close(socket_fd);
                exit(1);
            }
        //faltan las otras dos colas

        cliente[0] = thread(&tareas, ref(socTareas), client_fd[0], monitores[0]);
        cout << "Nuevo cliente " + to_string(0) + " aceptado" + "\n";
        cliente[1] = thread(&tags, monitores[1]);
        cout << "Nuevo cliente " + to_string(1) + " aceptado" + "\n";
        cliente[2] = thread(&QoS, monitores[2]);
        cout << "Nuevo cliente " + to_string(2) + " aceptado" + "\n";


        for (int i = 0; i < N_COLAS; i++) {
            cliente[i].join();
        }

        // Cerramos el socket de tareas
        error_code = socTareas.Close(socket_fd);
        if (error_code == -1) {
            cerr << "Error cerrando el socket de tareas: " + string(strerror(errno)) + " aceptado" + "\n";
        }
        //falta cerrar los otros dos sockets    
    }
    else {
        cout << "Ejecutar de la siguiente forma:" << endl;
        cout << "./gestorDeColas XXXX ZZZZ" << endl;
        cout << "siendo XXXX el número del puerto a emplear para comunicarse con masterWorker" << endl;
        cout << "siendo ZZZZ el número del puerto a emplear para comunicarse con los analizadores" << endl;
    }
    return 0;
}