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
#include "Socket/Socket.hpp"
#include "MWprocesado.hpp"
#include <thread>
#include <ctime>
using namespace std;

void master(int PORT_STREAMING, string IP_STREAMING, int PORT_GESTOR, string IP_GESTOR) {
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
    cout << "CONEXION STREAMING ESTABLECIDA" << endl;
    // Conectamos con el servidor. Probamos varias conexiones
    count = 0;
    cout << "CONECTANDO CON GESTOR" << endl;
    int socket_fd_gestor;
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
    cout << "CONEXION GESTOR ESTABLECIDA" << endl;
    int LENGTH = 500;
    string tareas[5];
    string mensaje;
    string aux;
    int read_bytes;   //num de bytes recibidos en un mensaje
    int send_bytes;  //num de bytes enviados en un mensaje
    int len;
    int n;
    for(int j = 0; j < 3; j++) {
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
        for(int i = 0; i < 25; i++) {
             // Recibimos la respuesta del servidor de streaming
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
            len = tareas[i].length();
            n = len/500;
            for(int k = 0; k < 25; k++) {
                // Enviamos el bloque de tareas al gestor de colas
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
        // Enviamos el mensaje de fin al servicio gestor de colas
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
    cout << "CONEXIONES FINALIZADAS" << endl;
}

void worker(int PORT_GESTOR, string IP_GESTOR, int id) {
    string MENS_FIN = "FIN";
    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor gestor.
    Socket chanGestor(IP_GESTOR, PORT_GESTOR);
    cout << "CONECTANDO CON GESTOR, " << id << endl;
    // Conectamos con el servidor. Probamos varias conexiones
    const int MAX_ATTEMPS = 10;
    int count = 0;
    int socket_fd_gestor;
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
    cout << "CONEXION ESTABLECIDA" << endl;
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
            cerr << "Error al enviar datos fin al gestor: " << strerror(errno) << endl;
            // Cerramos el socket
            chanGestor.Close(socket_fd_gestor);
            exit(1);
        }
        for(int i = 0; i < 24; i++) {
            // Enviamos el mensaje de fin al servicio gestor de colas
            send_bytes = chanGestor.Send(socket_fd_gestor, "$$");
                    
            if(send_bytes == -1) {
                cerr << "Error al enviar datos fin al gestor: " << strerror(errno) << endl;
                // Cerramos el socket
                chanGestor.Close(socket_fd_gestor);
                exit(1);
            }
        }
        cout << "PETICION DE LECTURA DE TAREAS ENVIADA, " << id << endl;
        mensaje = "";
        for(int i = 0; i < 25; i++) {
            // Recibimos la respuesta del servidor gestor
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
            proccessTaskBlock(mensaje,perf,tags);
            cout << "BLOQUE DE TAREAS PROCESADO" << id << endl;
            // Enviamos el mensaje de petición al gestor
            mensaje = "PUBLISH_QoS," + perf + "," + to_string(id);
            len = mensaje.length();
            n = len/500;
            for(int k = 0; k < 25; k++) {
                if(k <  n) send_bytes = chanGestor.Send(socket_fd_gestor, mensaje.substr(k*500,500));
                else if(k == n) send_bytes = chanGestor.Send(socket_fd_gestor, mensaje.substr(k*500,500)+"$$");
                else send_bytes = chanGestor.Send(socket_fd_gestor, " ");
                if(send_bytes == -1) {
                    cerr << "Error al enviar datos al gestor: " << strerror(errno) << endl;
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
                else if(k == n) send_bytes = chanGestor.Send(socket_fd_gestor, mensaje.substr(k*500,500)+"$$");
                else send_bytes = chanGestor.Send(socket_fd_gestor, " ");
                if(send_bytes == -1) {
                    cerr << "Error al enviar datos al gestor: " << strerror(errno) << endl;
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
    cout << "CONEXION FINALIZADA" << endl;
}

int main(int argc, char* argv[]) {
    if(argc == 5) {
        //VARIABLES DE INVOCACIÓN
        int PORT_STREAMING = stoi(argv[1]);
        string IP_STREAMING = string(argv[2]);
        int PORT_GESTOR[N_WORKERS + 1];
        PORT_GESTOR[N_WORKERS] = stoi(argv[3]);
        for(int i = 0; i < N_WORKERS; i++) PORT_GESTOR[i] = PORT_GESTOR[N_WORKERS];
        string IP_GESTOR[N_WORKERS + 1];
        IP_GESTOR[N_WORKERS] = string(argv[4]);
        for(int i = 0; i < N_WORKERS; i++) IP_GESTOR[i] = IP_GESTOR[N_WORKERS];
        //CREACION DE THREADS
        thread mast;
        thread workers[N_WORKERS];
        cout << "THREADS INICIANDOSE" << endl;
        mast = thread(&master, PORT_STREAMING, IP_STREAMING, PORT_GESTOR[N_WORKERS], IP_GESTOR[N_WORKERS]);
        for(int i = 0; i < N_WORKERS; i++) {
            workers[i] = thread(&worker,PORT_GESTOR[i], IP_GESTOR[i], i);
        }
        cout << "THREADS INICIADOS" << endl;
        //ESPERA FINALIZACIÓN
        mast.join();
        for(int i = 0; i < N_WORKERS; i++) {
            workers[i].join();
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
    return 0;
}