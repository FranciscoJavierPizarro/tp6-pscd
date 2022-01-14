//------------------------------------------------------------------------------
// File:   streaming.cpp
// Authors: Axel Isaac Pazmiño Ortega NIP: 817627 y Alicia Lázaro Huerta NIP: 820574
// Date:    19/12/2021
// Coms:   Código principal del servicio de streaming
//         Emplea el fichero resultante de ejecutar el script filtro
//         Se comunica con el programa masterWorker
// Use:    Para usarlo se debe invocar de la siguiente manera
//         ./streaming XXXX
//         siendo XXXX el número del puerto a emplear
//------------------------------------------------------------------------------
#include <iostream>
#include <string.h>
#include <fstream>
#include "Socket/Socket.hpp"
using namespace std;


//-------------------------------------------------------------
// Pre:---
// Post: Devuelve 25 tweets de tweets-filtrados.csv en buffer
void getTweets(string tweets[25],ifstream& f) {
    int maxTweets = 25;
    string aux;
    for (int i = 0; i < maxTweets; i++) {
        getline(f,aux);
        // en caso de llegar al final del fichero:
        if (f.eof()) {
            f.clear();
            f.seekg (0, ios::beg);
            string cabecera;
            getline(f,cabecera); // ignoramos la cabecera
        }
        else {
            tweets[i] = "$" + to_string(i) + " " + aux;
        }
    }
}
//-------------------------------------------------------------

int main(int argc, char* argv[]) {
    if(argc == 2) {
        // Puerto donde escucha el proceso servidor
        int PORT = stoi(argv[1]);

        string MENS_FIN = "FIN";
        string TWEETS = "tweets-filtrados.csv";

        //abrimos el fichero del cual vamos a extaer los tweets:
        ifstream f;
        f.open(TWEETS);
        string cabecera;
        getline(f,cabecera); // ignoramos la cabecera
        // Creación del socket con el que se llevará a cabo
        // la comunicación con el servidor.
        Socket chan(PORT);

        // Bind
        const int MAX_ATTEMPS = 15;
        int count = 0;
        int socket_fd;
        do {
            socket_fd = chan.Bind();
            if (socket_fd == -1) {
                cerr << "Error en el bind: " + string(strerror(errno)) + "\n";
                sleep(count);
                cerr << "REINTENTANDO" << endl;
                count++;
            }
        }while(socket_fd == -1 && count < MAX_ATTEMPS);
        if (socket_fd == -1) {
            string mensError(strerror(errno));
            cerr << "Error en el bind: " + mensError + "\n";
            exit(1);
        }

        // Listen
        int max_connections = 1;
        int error_code = chan.Listen(max_connections);
        if (error_code == -1) {
            string mensError(strerror(errno));
            cerr << "Error en el listen: " + mensError + "\n";
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }
        cout << "\033[1;4;5mESCUCHANDO SOCKET\033[0m" << endl;
        // Accept
        int client_fd = chan.Accept();
        if (client_fd == -1) {
            string mensError(strerror(errno));
            cerr << "Error en el accept: " + mensError + "\n";
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }
        cout << "\033[32;1;4;5mCONEXION ESTABLECIDA\033[0m" << endl;
        // Buffer para recibir el mensaje
        int length = 50;
        string buffer;
        int rcv_bytes;   //num de bytes recibidos en un mensaje
        int send_bytes;  //num de bytes enviados en un mensaje
        int len;
        bool out = false; // Inicialmente no salir del bucle
        while (!out) {
            // Recibimos el mensaje del cliente
            rcv_bytes = chan.Recv(client_fd, buffer,length);
            if(rcv_bytes == -1) {
                string mensError(strerror(errno));
                cerr << "Error al recibir datos: " + mensError + "\n";
                // Cerramos los sockets
                chan.Close(client_fd);
                chan.Close(socket_fd);
            }

            // Si recibimos "FIN" --> Fin de la comunicación
            if (buffer == MENS_FIN) {
                out = true; // Salir del bucle
            } else {
                cout << "TWEETS SOLICITADOS" << endl;
                string tweets[25];
                // colocamos 25 tweets en el buffer
                getTweets(tweets,f);
                for(int i = 0; i < 25; i++) {
                    // Enviamos la respuesta
                    send_bytes = chan.Send(client_fd, tweets[i]);
                    // send_bytes = chan.Send(client_fd, " ");
                    if(send_bytes == -1) {
                        string mensError(strerror(errno));
                        cerr << "Error al enviar datos: " + mensError + "\n";
                        // Cerramos los sockets
                        chan.Close(client_fd);
                        chan.Close(socket_fd);
                        exit(1);
                    }
                }
                cout << "TWEETS ENVIADOS" << endl;
            }
        }

        // Cerramos el socket del cliente
        error_code = chan.Close(client_fd);
        if (error_code == -1) {
            string mensError(strerror(errno));
            cerr << "Error cerrando el socket del cliente: " + mensError + "\n";
        }
        cout << "\033[32;1;4;5mCONEXION FINALIZADA\033[0m" << endl;
        // Cerramos el socket del servidor
        error_code = chan.Close(socket_fd);
        if (error_code == -1) {
            string mensError(strerror(errno));
            cerr << "Error cerrando el socket del servidor: " + mensError + "\n";
        }
        // Mensaje de despedida
        cout << "BYE BYE" << endl;

        return error_code;
    }
    else {
        cout << "Ejecutar de la siguiente forma:" << "\n";
        cout << "./streaming XXXX" << "\n";
        cout << "siendo XXXX el número del puerto a emplear" << endl;
    }
    return 0;
}