//------------------------------------------------------------------------------
// File:   gestorDeColas.cpp
// Author: Pablo López Mosqueda
//         Leonor Murphy Anía
//         Francisco Javier Pizarro
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

const int N = 5;
const int N_COLAS = 3;

//Comms:Se encarga de gestionar todo el funcionamiento/comunicación con un proceso master o worker
void masterWorker(Socket& socTareas, int fd, ControldeCola& controlTareas, BoundedQueue<string>& colaTareas,
                  ControldeCola& controlQoS, BoundedQueue<string>& colaQoS, ControldeCola& controlTags, 
                  BoundedQueue<string>& colaTags, bool finalizados[N], int n){
    //VARIABLES PARA RECIBIR/PROCESAR/ENVIAR MENSAJES
    int length = 500;
    string mensaje, datos;
    string temp;
    bool out = false;
    string aux;
    int n_len, len;
    int send_bytes, rcv_bytes;

    while(!out){
        //Recibir petición
        for(int i = 0; i < 25; i++) {
            rcv_bytes = socTareas.Recv(fd,aux,length);
            if (rcv_bytes == -1) {
                cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
                // Cerramos los sockets
                socTareas.Close(fd);
            }
            if(i == 0) mensaje = aux;
            else mensaje.append(aux);
        }
        mensaje = mensaje.substr(0,mensaje.find("$$"));
        
        //Procesar petición
        if(mensaje == "FIN") {
            cout << "FIN PROCESO MASTER RECIBIDO" << endl;
            out = true;
            finalizados[n] = true;
            controlTareas.finalizar();
        }
        else {
            datos = mensaje.substr(mensaje.find(",") + 1,(mensaje.length() - mensaje.find(",")));
            mensaje = mensaje.substr(0,mensaje.find(","));
            if(mensaje == "PUBLISH_TAREAS") {
                controlTareas.escribirCola(colaTareas, datos);
            }
            else if(mensaje == "READ_TAREAS") {
                if(controlTareas.leerCola(colaTareas, datos)) {
                    //Enviar respuesta con bloque de tareas
                    len = datos.length();
                    n_len = len/500;
                    for(int k = 0; k < 25; k++) {
                        if(k <  n_len) send_bytes = socTareas.Send(fd, datos.substr(k*500,500));
                        else if(k == n_len) send_bytes = socTareas.Send(fd, datos.substr(k*500,498)+"$$");
                        else send_bytes = socTareas.Send(fd, " ");
                        if(send_bytes == -1) {
                            cerr << "Error al enviar datos al gestor: " << strerror(errno) << endl;
                            cerr << "iteracion:" << k << " valor de n_len: " << n_len;
                            // Cerramos el socket
                            socTareas.Close(fd);
                            exit(1);
                        }
                    }
                }
                else {
                    //No quedan mas bloques de tareas para procesar, avisamos al worker de que finalice
                    mensaje = "FIN";
                    //Enviar mensaje
                    send_bytes = socTareas.Send(fd, mensaje);
                    if(send_bytes == -1) {
                        string mensError(strerror(errno));
                        cerr << "Error al enviar datos: " + mensError + "\n";
                        // Cerramos los sockets
                        socTareas.Close(fd);
                        exit(1);
                    }
                    for(int i = 0; i < 24; i++) {
                    send_bytes = socTareas.Send(fd, "$$");
                        if(send_bytes == -1) {
                            string mensError(strerror(errno));
                            cerr << "Error al enviar datos: " + mensError + "\n";
                            // Cerramos los sockets
                            socTareas.Close(fd);
                            exit(1);
                        }
                    }
                    cout << "FIN ENVIADO A PROCESO WORKER" << endl;
                    finalizados[n] = true;
                    out = true;
                }
            }
            else if(mensaje == "PUBLISH_QoS") {
                controlQoS.escribirCola(colaQoS, datos);
            }
            else if(mensaje == "PUBLISH_TAGS"){
                controlTags.escribirCola(colaTags, datos);
            }
        }
    }
    //Comprobamos si todos los procesos master-worker han finalizado
    //para marcar el fin en las colas Qos y Tareas
    bool auxb = finalizados[0];
    for(int i = 1; i < N + 1; i++) {
        auxb = auxb && finalizados[i];
    }
    if(auxb) {
        controlQoS.finalizar();
        controlTags.finalizar();
    }
    socTareas.Close(fd); //cerrar el socket
    cout << "\033[32;1;4mCONEXION FINALIZADA\033[0m" << endl;
    }

//Comms:Se encarga de gestionar todo el funcionamiento/comunicación de los analizadores
void analizadores(Socket& socAnalizadores, int fd, ControldeCola& controlQoS, BoundedQueue<string>& colaQoS, 
                  ControldeCola& controlTags, BoundedQueue<string>& colaTags){
    //VARIABLES PARA RECIBIR/PROCESAR/ENVIAR MENSAJES
    int length = 500;
    string mensaje, datos;
    bool out = false;
    string aux;
    int rcv_bytes, send_bytes, len, n_len;

    while(!out){
        //Recibir petición
        for(int i = 0; i < 25; i++) {
            rcv_bytes = socAnalizadores.Recv(fd,aux,length);
            if (rcv_bytes == -1) {
                cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
                // Cerramos los sockets
                socAnalizadores.Close(fd);
            }
            if(i == 0) mensaje = aux;
            else mensaje.append(aux);
        }
        mensaje = mensaje.substr(0,mensaje.find("$$"));

        //Extraer información de la petición
        datos = mensaje.substr(mensaje.find(",") + 1,(mensaje.length() - mensaje.find(",")));
        mensaje = mensaje.substr(0,mensaje.find(","));
        
        if(mensaje == "READ_QoS") {
            if(controlQoS.leerCola(colaQoS, datos)){
                //Enviar datosQos
                len = datos.length();
                n_len = len/500;
                for(int k = 0; k < 25; k++) {
                    if(k <  n_len) send_bytes = socAnalizadores.Send(fd, datos.substr(k*500,500));
                    else if(k == n_len) send_bytes = socAnalizadores.Send(fd, datos.substr(k*500,498)+"$$");
                    else send_bytes = socAnalizadores.Send(fd, " ");
                    if(send_bytes == -1) {
                        cerr << "Error al enviar datos al gestor: " << strerror(errno) << endl;
                        cerr << "iteracion:" << k << " valor de n_len: " << n_len;
                        // Cerramos el socket
                        socAnalizadores.Close(fd);
                        exit(1);
                    }
                }
            }
            else {
                //Avisar al analizador de rendimiento de que finalice porque no quedan datos
                mensaje = "FIN";
                cout << "FINALIZANDO ANALIZADOR RENDIMIENTO" << endl;
                //Enviar mensaje
                send_bytes = socAnalizadores.Send(fd, mensaje);
                if(send_bytes == -1) {
                    string mensError(strerror(errno));
                    cerr << "Error al enviar datos: " + mensError + "\n";
                    // Cerramos los sockets
                    socAnalizadores.Close(fd);
                    exit(1);
                }
                for(int i = 0; i < 24; i++) {
                    send_bytes = socAnalizadores.Send(fd, "$$");
                    if(send_bytes == -1) {
                        string mensError(strerror(errno));
                        cerr << "Error al enviar datos: " + mensError + "\n";
                        // Cerramos los sockets
                        socAnalizadores.Close(fd);
                        exit(1);
                    }
                }
                out = true;
            }
        }
        else if(mensaje == "READ_TAGS") {
            if(controlTags.leerCola(colaTags, datos)) {
                //Enviar datos tags
                len = datos.length();
                n_len = len/500;
                for(int k = 0; k < 25; k++) {
                    if(k <  n_len) send_bytes = socAnalizadores.Send(fd, datos.substr(k*500,500));
                    else if(k == n_len) send_bytes = socAnalizadores.Send(fd, datos.substr(k*500,498)+"$$");
                    else send_bytes = socAnalizadores.Send(fd, " ");
                    if(send_bytes == -1) {
                        cerr << "Error al enviar datos al gestor: " << strerror(errno) << endl;
                        cerr << "iteracion:" << k << " valor de n_len: " << n_len;
                        // Cerramos el socket
                        socAnalizadores.Close(fd);
                        exit(1);
                    }
                }
            }
            else {
                //Avisar al analizador de tags de que finalice porque no quedan datos
                mensaje = "FIN";
                cout << "FINALIZANDO ANALIZADOR TAGS" << endl;
                //Enviar mensaje
                send_bytes = socAnalizadores.Send(fd, mensaje); //enviar el elemento leído
                if(send_bytes == -1) {
                    string mensError(strerror(errno));
                    cerr << "Error al enviar datos: " + mensError + "\n";
                    // Cerramos los sockets
                    socAnalizadores.Close(fd);
                    exit(1);
                }
                for(int i = 0; i < 24; i++) {
                    send_bytes = socAnalizadores.Send(fd, "$$");
                    if(send_bytes == -1) {
                        string mensError(strerror(errno));
                        cerr << "Error al enviar datos: " + mensError + "\n";
                        // Cerramos los sockets
                        socAnalizadores.Close(fd);
                        exit(1);
                    }
                }
                out = true;
            }
        } 
    }
    socAnalizadores.Close(fd); //cerrar el socket
    cout << "\033[32;1;4mCONEXION FINALIZADA\033[0m" << endl;
}

int main(int argc, char* argv[]) {
    if(argc == 3) {
        //VARIABLES PARA COMUNICACIÓN
        int PORT_MASTERWORKER = stoi(argv[1]);
        int PORT_ANALIZADORES = stoi(argv[2]);
        //VARIABLES PARA CONCURRENCIA
        ControldeCola monitorTareas;
        ControldeCola monitorQoS;
        ControldeCola monitorTags;
        BoundedQueue<string> colaTareas(500);
        BoundedQueue<string> colaQoS(500);
        BoundedQueue<string> colaTags(500);
        bool finalizado[N + 1];
        for(int i = 0; i < N + 1;i++) {
            finalizado[i] = false;
        }
        //MASTER WORKERS INICIANDOSE
        int MW_fd[N + 1];
        thread MW[N + 1];
        // Creación del canal masterWorker.
        Socket chanMasterWorker(PORT_MASTERWORKER);
        // Bind
        int socket_fd_masterWorker;
        const int MAX_ATTEMPS = 15;
        int count = 0;
        do {
            socket_fd_masterWorker = chanMasterWorker.Bind();
            if (socket_fd_masterWorker == -1) {
                cerr << "Error en el bind: " + string(strerror(errno)) + "\n";
                sleep(count);
                cerr << "REINTENTANDO" << endl;
                count++;
            }
        }while(socket_fd_masterWorker == -1 && count < MAX_ATTEMPS);
        if (socket_fd_masterWorker == -1) {
            cerr << "Error en el bind: " + string(strerror(errno)) + "\n";
            exit(1);
        }
        // Listen
        int error_code1 = chanMasterWorker.Listen(N);
        if (error_code1 == -1) {
            cerr << "Error en el listen: " + string(strerror(errno)) + "\n";
            // Cerramos el socket
            chanMasterWorker.Close(socket_fd_masterWorker);
            exit(1);
        }
        cout << "\033[1;4mESCUCHANDO SOCKET MASTER-WORKERS\033[0m" << endl;
        for (int i=0; i<N + 1; i++) {
            // Accept
            MW_fd[i] = chanMasterWorker.Accept();
            if(MW_fd[i] == -1) {
                cerr << "Error en el accept: " + string(strerror(errno)) + "\n";
                // Cerramos el socket
                chanMasterWorker.Close(socket_fd_masterWorker);
                exit(1);
            }
            cout << "\033[32;1;4mCONEXION ESTABLECIDA\033[0m" << endl;
            MW[i] = thread(&masterWorker, ref(chanMasterWorker), MW_fd[i], ref(monitorTareas), ref(colaTareas), 
                           ref(monitorQoS), ref(colaQoS), ref(monitorTags), ref(colaTags), ref(finalizado), i);
        }

        //ANALIZADORES INICIANDOSE
        int fd_analizadores[2];
        thread Tanalizadores[2];
        // Creación del canal analizadores.
        Socket chanAnalizadores(PORT_ANALIZADORES);
        // Bind
        int socket_fd_Analizadores;
        count = 0;
        do {
            socket_fd_Analizadores = chanAnalizadores.Bind();
            if (socket_fd_Analizadores == -1) {
                cerr << "Error en el bind: " + string(strerror(errno)) + "\n";
                sleep(count);
                cerr << "REINTENTANDO" << endl;
                count++;
            }
        }while(socket_fd_Analizadores == -1 && count < MAX_ATTEMPS);
        if (socket_fd_Analizadores == -1) {
            cerr << "Error en el bind: " + string(strerror(errno)) + "\n";
            exit(1);
        }
        // Listen
        int error_code2 = chanAnalizadores.Listen(2);
        if (error_code2 == -1) {
            cerr << "Error en el listen: " + string(strerror(errno)) + "\n";
            // Cerramos el socket
            chanAnalizadores.Close(socket_fd_Analizadores);
            exit(1);
        }
        cout << "\033[1;4mESCUCHANDO SOCKET ANALIZADORES\033[0m" << endl;

        for(int i = 0; i < 2; i++) {
            // Accept
            fd_analizadores[i] = chanAnalizadores.Accept();

            if(fd_analizadores[i] == -1) {
                cerr << "Error en el accept: " + string(strerror(errno)) + "\n";
                // Cerramos el socket
                chanAnalizadores.Close(socket_fd_Analizadores);
                exit(1);
            }
            cout << "\033[32;1;4mCONEXION ESTABLECIDA\033[0m" << endl;
            Tanalizadores[i] = thread(&analizadores, ref(chanMasterWorker), fd_analizadores[i], ref(monitorQoS), ref(colaQoS),
                                    ref(monitorTags), ref(colaTags));
        }
        //Esperamos a que acaben todos los threads
        for (int i=0; i<N + 1; i++) {
            MW[i].join();
        }
        for (int i=0; i<2; i++) {
            Tanalizadores[i].join();
        }
        // Cerramos el socket del servidor
        error_code1 = chanMasterWorker.Close(socket_fd_masterWorker);
        if (error_code1 == -1) {
            cerr << "Error cerrando el socket del servidor: " + string(strerror(errno)) + " aceptado" + "\n";
        }
        
        // Cerramos el socket del servidor
        error_code2 = chanAnalizadores.Close(socket_fd_Analizadores);
        if (error_code2 == -1) {
            cerr << "Error cerrando el socket del servidor: " + string(strerror(errno)) + " aceptado" + "\n";
        }
        cout << "BYE BYE" << endl;
    }
    else {
        cout << "Ejecutar de la siguiente forma:" << "\n";
        cout << "./gestorDeColas XXXX ZZZZ" << "\n";
        cout << "siendo XXXX el número del puerto a emplear para comunicarse con masterWorker" << "\n";
        cout << "siendo ZZZZ el número del puerto a emplear para comunicarse con los analizadores" << endl;
        return 0;
    }
}