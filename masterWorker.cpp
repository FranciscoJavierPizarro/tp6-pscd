//------------------------------------------------------------------------------
// File:   masterWorker.cpp
// Authors:Inés Román Gracia
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
Procesamiento de las respuestas master hecho
Falta concretar/implementar que deben enviar los workers
y comentar el código


*/
//CONSTANTES
const int N_WORKERS = 5;
const int TWEETS_FROM_STREAM = 25;
const int TWEETS_TO_TASK = 5;

//Comm: Procesa el bloque de entrada y genera los bloques de tareas
//Pre:  inTweets contiene un string formado por tantos tweets como TWEETS_FROM_STREAM
//      cada tweet tiene a su izquierda "$i" i € [0,TWEETS_FROM_STREAM) y a su derecha
//      "$j" j € (0,TWEETS_FROM_STREAM]
//Post: para cada i € [0, TWEETS_FROM_STREAM/TWEETS_TO_TASK), outTasks[i] contiene:
//      un string formado por tantos tweets como TWEETS_TO_TASK
//      cada tweet tiene a su izquierda "$i" i € [0,TWEETS_TO_TASK) y a su derecha
//      "$j" j € (0,TWEETS_TO_TASK], esto esta precedido por "PUBLISH_TAREAS,"
void createTasksBlock(string inTweets, string outTasks[TWEETS_FROM_STREAM/TWEETS_TO_TASK]) {
    for(int i = 0; i < TWEETS_FROM_STREAM/TWEETS_TO_TASK; i++) {
        outTasks[i] = "PUBLISH_TAREAS,";
    }
    int a, b;
    for(int i = 0; i < TWEETS_FROM_STREAM; i++) {
        a = (inTweets.find("$" + to_string(i)))+("$" + to_string(i)).length();
        b = (inTweets.find_first_of("$",a)); 
        outTasks[i/TWEETS_TO_TASK].append("$" + to_string((i) % TWEETS_TO_TASK) + inTweets.substr(a,b - a));
        if ((i + 1) % TWEETS_TO_TASK == 0) outTasks[i/TWEETS_TO_TASK].append("$" + to_string(TWEETS_TO_TASK));
    }
}

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

    for(int j = 0; j < 1; j++) {
        mensaje = "GET_TWEETS";
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
        createTasksBlock(mensaje,tareas);

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
    error_code = chanGestor.Close(socket_fd_gestor);
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
        //VARIABLES DE INVOCACIÓN
        int PORT_STREAMING = stoi(argv[1]);
        string IP_STREAMING = string(argv[2]);
        int PORT_GESTOR = stoi(argv[3]);
        string IP_GESTOR = string(argv[4]);
        //string prueba = "$0a$1aa$2aaa$3aaaa$4aaaaa$5b$6bb$7bbb$8bbbb$9bbbbb$10c$11cc$12ccc$13cccc$14ccccc$15d$16dd$17ddd$18dddd$19ddddd$20e$21ee$22eee$23eeee$24eeeee$25";
        //string prueba2[5];
        //createTasksBlock(prueba,prueba2);
        //CREACION DE THREADS
        thread mast;
        thread workers[N_WORKERS];
        mast = thread(&master, PORT_STREAMING, IP_STREAMING, PORT_GESTOR, IP_GESTOR);
        for(int i = 0; i < N_WORKERS; i++) {
            workers[i] = thread(&worker,PORT_GESTOR, IP_GESTOR, i);
        }
        //ESPERA FINALIZACIÓN
        mast.join();
        for(int i = 0; i < N_WORKERS; i++) {
            workers[i].join();
        }
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