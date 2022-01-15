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
#include "Semaphore_V4/Semaphore_V4.hpp"
#include "Socket/Socket.hpp"
#include "MWprocesado.hpp"
#include <thread>
#include <ctime>
using namespace std;

//FUNCIÓN MASTER
//Se encarga de solicitar paquetes de 25 tweets al servicio de streaming y de publicar paquetes de 5 en el gestor de colas
//como tareas
void master(int PORT_STREAMING, string IP_STREAMING, int PORT_GESTOR, string IP_GESTOR, Semaphore& sem) {
    string MENS_FIN = "FIN";
    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chanGestor(IP_GESTOR, PORT_GESTOR);

    // Conectamos con el servidor. Probamos varias conexiones
    const int MAX_ATTEMPS = 10;
    int count = 0;
    cout << "CONECTANDO CON STREAMING" << endl;
    Socket chanStream(IP_STREAMING, PORT_STREAMING);

    int socket_fd_streaming;
    do {
        // Conexión con el servidor
        socket_fd_streaming = chanStream.Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if(socket_fd_streaming == -1) {
            this_thread::sleep_for(chrono::seconds(1));
            cerr << "Error al conectar con el streaming, " << count  << endl;
        }
    } while(socket_fd_streaming == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if(socket_fd_streaming == -1) {
        cerr << "Error al conectar con el streaming: " << strerror(errno) << endl;
        // Cerramos el socket
        chanStream.Close(socket_fd_streaming);
        exit(1);   
    }
    cout << "\033[32;1;4;5mCONEXION STREAMING ESTABLECIDA\033[0m" << endl;
    // Conectamos con el servidor. Probamos varias conexiones
    count = 0;
    cout << "CONECTANDO CON GESTOR" << endl;
    int socket_fd_gestor;
    sem.wait();//Por un bug relativo a los puertos hacemos que esta conexión
    //se realice en exclusión mutua
    do {
        // Conexión con el servidor
        socket_fd_gestor = chanGestor.Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if(socket_fd_gestor == -1) {
            this_thread::sleep_for(chrono::seconds(1));
            cerr << "Error al conectar con el gestor, " << count << endl;
        }
    } while(socket_fd_gestor == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if(socket_fd_gestor == -1) {
        cerr << "Error al conectar con el gestor: " << strerror(errno) << endl;
        // Cerramos el socket
        chanGestor.Close(socket_fd_gestor);
        exit(1);   
    }
    sem.signal();
    cout << "\033[32;1;4;5mCONEXION GESTOR ESTABLECIDA\033[0m" << endl;

    //VARIABLES PARA RECIBIR/PROCESAR/ENVIAR MENSAJES
    int LENGTH = 500;
    string tareas[5];
    string mensaje;
    string aux;
    int read_bytes;   //num de bytes recibidos en un mensaje
    int send_bytes;  //num de bytes enviados en un mensaje
    int len;
    int n;
    for(int j = 0; j < 200; j++) {
        mensaje = "GET_TWEETS";
        // Enviamos el mensaje de petición al servicio de streaming
        send_bytes = chanStream.Send(socket_fd_streaming, mensaje);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos de solicitar tweets al streaming: " << strerror(errno) << endl;
            // Cerramos el socket
            chanStream.Close(socket_fd_streaming);
            exit(1);
        }
        cout << "TWEETS SOLICITADOS" << endl;
        mensaje = "";


        // Recibimos la respuesta del servidor de streaming
        for(int i = 0; i < 25; i++) {
            read_bytes = chanStream.Recv(socket_fd_streaming, aux, LENGTH);
            if(read_bytes == -1) {
                cerr << "Error al recibir datos: " << strerror(errno) << endl;
                // Cerramos el socket
                chanStream.Close(socket_fd_streaming);
                exit(1);
            }
            mensaje.append(aux);    
        }
        cout << "TWEETS RECIBIDOS" << endl;
        //PROCESAR RESPUESTA
        createTasksBlock(mensaje,tareas);

        for(int i = 0; i < 5; i++) {
            // Enviamos el bloque de tareas al gestor de colas
            len = tareas[i].length();
            n = len/500;
            for(int k = 0; k < 25; k++) {
                if(k <  n) send_bytes = chanGestor.Send(socket_fd_gestor, tareas[i].substr(k*500,500));
                else if(k == n) send_bytes = chanGestor.Send(socket_fd_gestor, tareas[i].substr(k*500,500)+"$$");
                // send_bytes = chanGestor.Send(socket_fd_gestor, tareas[i].substr(k*len/26,len/26));
                else send_bytes = chanGestor.Send(socket_fd_gestor, " ");
                if(send_bytes == -1) {
                    cerr << "Error al enviar datos al gestor: " << strerror(errno) << endl;
                    // Cerramos el socket
                    chanGestor.Close(socket_fd_gestor);
                    exit(1);
                }
            }
        }
        cout << "BLOQUES DE TAREAS PUBLICADOS" << endl;
    }
    // Enviamos el mensaje de fin al servicio de streaming
    send_bytes = chanStream.Send(socket_fd_streaming, MENS_FIN);        
    if(send_bytes == -1) {
        cerr << "Error al enviar datos fin al straming: " << strerror(errno) << endl;
        // Cerramos el socket
        chanStream.Close(socket_fd_streaming);
        exit(1);
    }

    // Enviamos el mensaje de fin al servicio gestor de colas
    send_bytes = chanGestor.Send(socket_fd_gestor, MENS_FIN);            
    if(send_bytes == -1) {
        cerr << "Error al enviar datos fin al gestor: " << strerror(errno) << endl;
        // Cerramos el socket
        chanGestor.Close(socket_fd_gestor);
        exit(1);
    }
    for(int i = 0; i < 24; i++) {
        send_bytes = chanGestor.Send(socket_fd_gestor, "$$");        
        if(send_bytes == -1) {
            cerr << "Error al enviar datos fin al gestor: " << strerror(errno) << endl;
            // Cerramos el socket
            chanGestor.Close(socket_fd_gestor);
            exit(1);
        }
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
    cout << "\033[32;1;4;5mCONEXIONES FINALIZADAS\033[0m" << endl;
}

//FUNCIÓN WORKER
//Se encarga de solicitar paquetes de tareas al gestor de colas y de publicar sus resultados
//en el gestor de colas
void worker(int PORT_GESTOR, string IP_GESTOR, int id, Semaphore& sem) {
    string MENS_FIN = "FIN";
    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor gestor.
    Socket chanGestor(IP_GESTOR, PORT_GESTOR);
    cout << "CONECTANDO CON GESTOR, " << id << endl;
    // Conectamos con el servidor. Probamos varias conexiones
    const int MAX_ATTEMPS = 10;
    int count = 0;
    int socket_fd_gestor;
    sem.wait();//Por un bug relativo a los puertos hacemos que esta conexión
    //se realice en exclusión mutua
    do {
        // Conexión con el servidor
        socket_fd_gestor = chanGestor.Connect();
        // cout << "INTENTO DE CONNECT HECHO" << endl;
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if(socket_fd_gestor == -1) {
            this_thread::sleep_for(chrono::seconds(1));
            cerr << "Error al conectar con el gestor," << id << " , " << count << endl;
        }
    } while(socket_fd_gestor == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if(socket_fd_gestor == -1) {
        cerr << "Error al conectar con el gestor: " << strerror(errno) << endl;
        // Cerramos el socket
        chanGestor.Close(socket_fd_gestor);
        exit(1);   
    }
    sem.signal();
    cout << "\033[32;1;4;5mCONEXION ESTABLECIDA\033[0m" << endl;

    //VARIABLES PARA RECIBIR/PROCESAR/ENVIAR MENSAJES
    int LENGTH = 500;
    string tweets[5];
    string mensaje = "";
    string perf, tags;
    string aux;
    int read_bytes;   //num de bytes recibidos en un mensaje
    int send_bytes;  //num de bytes enviados en un mensaje
    bool out = false;
    int len;
    int n;
    while(!out) {
        // Enviamos el mensaje de petición al servicio gestor
        mensaje = "READ_TAREAS," + to_string(id);
        send_bytes = chanGestor.Send(socket_fd_gestor, mensaje);
        if(send_bytes == -1) {
            cerr << "Error al enviar petición datos al gestor: " << strerror(errno) << endl;
            // Cerramos el socket
            chanGestor.Close(socket_fd_gestor);
            exit(1);
        }
        for(int i = 0; i < 24; i++) {
            send_bytes = chanGestor.Send(socket_fd_gestor, "$$");
                    
            if(send_bytes == -1) {
                cerr << "Error al enviar petición fin al gestor1: " << strerror(errno) << endl;
                // Cerramos el socket
                chanGestor.Close(socket_fd_gestor);
                exit(1);
            }
        }
        cout << "PETICION DE LECTURA DE TAREAS ENVIADA, " << id << endl;
        mensaje = "";
        
        // Recibimos la respuesta del servidor gestor
        for(int i = 0; i < 25; i++) {
            read_bytes = chanGestor.Recv(socket_fd_gestor, aux, LENGTH);
            if(read_bytes == -1) {
                cerr << "Error al recibir datos: " << strerror(errno) << endl;
                // Cerramos el socket
                chanGestor.Close(socket_fd_gestor);
                exit(1);
            }
            mensaje.append(aux);
        }
        cout << "BLOQUE DE TAREAS RECIBIDO, " << id << endl;
        mensaje = mensaje.substr(0,mensaje.find("$$"));

        if(mensaje == MENS_FIN) { 
            out = true;
        }
        else{
            //PROCESAR
            perf = "";
            tags = "";
            proccessTaskBlock(mensaje,perf,tags);
            cout << "BLOQUE DE TAREAS PROCESADO" << id << endl;

            // Enviamos el mensaje de petición al gestor
            mensaje = "PUBLISH_QoS," + perf + "," + to_string(id);
            len = mensaje.length();
            n = len/500;
            for(int k = 0; k < 25; k++) {
                if(k <  n) send_bytes = chanGestor.Send(socket_fd_gestor, mensaje.substr(k*500,500));
                else if(k == n) send_bytes = chanGestor.Send(socket_fd_gestor, mensaje.substr(k*500,498)+"$$");
                else send_bytes = chanGestor.Send(socket_fd_gestor, " ");
                if(send_bytes == -1) {
                    cerr << "Error al enviar datos al gestor1: " << strerror(errno) << endl;
                    // Cerramos el socket
                    chanGestor.Close(socket_fd_gestor);
                    exit(1);
                }
            }
            // Enviamos el mensaje de petición al gestor
            mensaje = "PUBLISH_TAGS,"+ tags + "," + to_string(id);
            len = mensaje.length();
            n = len/500;
            for(int k = 0; k < 25; k++) {
                if(k <  n) send_bytes = chanGestor.Send(socket_fd_gestor, mensaje.substr(k*500,500));
                else if(k == n) send_bytes = chanGestor.Send(socket_fd_gestor, mensaje.substr(k*500,498)+"$$");
                else send_bytes = chanGestor.Send(socket_fd_gestor, " ");
                if(send_bytes == -1) {
                    cerr << "Error al enviar datos al gestor2: " << strerror(errno) << endl;
                    // Cerramos el socket
                    chanGestor.Close(socket_fd_gestor);
                    exit(1);
                }
            }
            cout << "RESULTADOS BLOQUE DE TAREAS ENVIADOS, " << id << endl;
        }
    }

    // Cerramos el socket
    int error_code = chanGestor.Close(socket_fd_gestor);
    if(error_code == -1) {
        cerr << "Error cerrando el socket: " << strerror(errno) << endl;
    }
    cout << "\033[32;1;4;5mCONEXION FINALIZADA\033[0m" << endl;
}

int main(int argc, char* argv[]) {
    if(argc == 5) {
        //VARIABLES DE INVOCACIÓN
        int PORT_STREAMING = stoi(argv[1]);
        string IP_STREAMING = string(argv[2]);
        int PORT_GESTOR = stoi(argv[3]);
        string IP_GESTOR = string(argv[4]);
        //CREACION DE THREADS
        thread mast;
        thread workers[N_WORKERS];
        Semaphore sem(1);//Se usa para evitar un bug al crear la comunicación
        cout << "\033[1;4;5mTHREADS INICIANDOSE\033[0m" << endl;
        mast = thread(&master, PORT_STREAMING, IP_STREAMING, PORT_GESTOR, IP_GESTOR, ref(sem));
        for(int i = 0; i < N_WORKERS; i++) {
            workers[i] = thread(&worker,PORT_GESTOR, IP_GESTOR, i, ref(sem));
        }
        cout << "\033[1;4;5mTHREADS INICIADOS\033[0m" << endl;
        //ESPERA FINALIZACIÓN
        mast.join();
        for(int i = 0; i < N_WORKERS; i++) {
            workers[i].join();
        }
        cout << "\033[1;4;5mTHREADS FINALIZADOS\033[0m" << endl;
        cout << "BYE BYE" << endl;
    }
    else {
        cout << "Ejecutar de la siguiente forma:" << "\n";
        cout << "./masterWorker XXXX YYYYYYYYY ZZZZ WWWWWWWWW" << "\n";
        cout << "siendo XXXX el número del puerto a emplear para comunicarse con streaming" << "\n";
        cout << "siendo YYYYYYYYY la ip a emplear para comunicarse con streaming" << "\n";
        cout << "siendo ZZZZ el número del puerto a emplear para comunicarse con gestor de colas" << "\n";
        cout << "siendo WWWWWWWWW la ip a emplear para comunicarse con gestor de colas" << endl;
    }
    return 0;
}