//------------------------------------------------------------------------------
// File:   analizadorRendimiento.cpp
// Authors: Axel Isaac Pazmiño Ortega
//          Alicia Lázaro Huerta
//          Francisco Javier Pizarro Martínez
// Date:   23/12/2021
// Coms:   Código principal del programa analizadorRendimiento
//         Se comunica con gestorDeColas
// Use:    Para usarlo se debe invocar de la siguiente manera
//         ./analizadorTags XXXX YYYYYYYYY
//         siendo XXXX el número del puerto a emplear para comunicarse con gestor de colas
//         siendo YYYYYYYYY la ip a emplear para comunicarse con gestor de colas
//------------------------------------------------------------------------------
#include "Socket/Socket.hpp"
#include <thread>
#include <ctime>
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
        const string MESSAGE =  "READ_QoS";
        string respuesta,aux;
        int send_bytes,read_bytes, n, len;
        int LENGTH = 500;
        string idS, datosS;
        int id;
        float datos;


        const int N_WORKERS = 5;
        int nDatos = 0;
        int procesados[N_WORKERS];
        for(int i = 0; i < N_WORKERS; i++) procesados[i] = 0;
        float Max, Min, Media;
        float MaxV[N_WORKERS], MinV[N_WORKERS], MediaV[N_WORKERS];
        // Enviamos el mensaje de petición al servicio gestor
        len = MESSAGE.length();
        n = len/500;
        for(int k = 0; k < 25; k++) {
            if(k <  n) send_bytes = chanGestor.Send(socket_fd_gestor, MESSAGE.substr(k*500,500));
            else if(k == n) send_bytes = chanGestor.Send(socket_fd_gestor, MESSAGE.substr(k*500,500)+"$$");
            else send_bytes = chanGestor.Send(socket_fd_gestor, " ");
            if(send_bytes == -1) {
                cerr << "Error al enviar datos al gestor: " << strerror(errno) << endl;
                // Cerramos el socket
                chanGestor.Close(socket_fd_gestor);
                exit(1);
            }
        }

        bool out = false;
        while (!out) {
            respuesta = "";
            for(int i = 0; i < 25; i++) {
                // Recibimos la respuesta del servidor gestor
                read_bytes = chanGestor.Recv(socket_fd_gestor, aux, LENGTH);
                
                if(read_bytes == -1) {
                    cerr << "Error al recibir datos: " << strerror(errno) << endl;
                    // Cerramos el socket
                    chanGestor.Close(socket_fd_gestor);
                    exit(1);
                }
                respuesta.append(aux);
            }
            respuesta = respuesta.substr(0,respuesta.find("$$"));
            
            
            if (respuesta == MENS_FIN) {
                out = true;
            }

            else {
                // tratamiento de la información recibida
                datosS = respuesta.substr(0,respuesta.find(","));
                idS = respuesta.substr(respuesta.find(",") + 1, respuesta.length() - respuesta.find(","));
                datos = stof(datosS);
                id = stoi(idS);


                if (nDatos == 0) {
                    Max = datos;
                    Min = datos;
                    Media = datos;
                }
                else {
                    if (Max < datos) Max = datos;
                    if (Min > datos) Min = datos;
                    Media += datos;
                }
                if (procesados[id] == 0) {
                    MaxV[id] = datos;
                    MinV[id] = datos;
                    MediaV[id] = datos;
                }
                else {
                    if (MaxV[id] < datos) MaxV[id] = datos;
                    if (MinV[id] > datos) MinV[id] = datos;
                    MediaV[id] += datos;
                }
                nDatos++;
                procesados[id]++;
            }
            if(respuesta != MENS_FIN) {
                len = MESSAGE.length();
                n = len/500;
                for(int k = 0; k < 25; k++) {
                    if(k <  n) send_bytes = chanGestor.Send(socket_fd_gestor, MESSAGE.substr(k*500,500));
                    else if(k == n) send_bytes = chanGestor.Send(socket_fd_gestor, MESSAGE.substr(k*500,500)+"$$");
                    else send_bytes = chanGestor.Send(socket_fd_gestor, " ");
                    if(send_bytes == -1) {
                        cerr << "Error al enviar datos al gestor: " << strerror(errno) << endl;
                        // Cerramos el socket
                        chanGestor.Close(socket_fd_gestor);
                        exit(1);
                    }
                }
            }
        }
        Media = Media / nDatos;
        for(int i = 0; i < N_WORKERS; i++) MediaV[i] = MediaV[i] / procesados[i];
        cout << "RESULTADOS GENERALES" << "\n";
        cout << "============================================" << "\n";
        cout << "Tiempo de procesado medio general: " << Media << "\n";
        cout << "Tiempo máximo de procesado general: " << Max << "\n";
        cout << "Tiempo mínimo de procesado general: " << Min << "\n";
        cout << "Número de paquetes procesados: " << nDatos << "\n";
        cout << "============================================" << endl;
        cout << "RESULTADOS INDIVIDUALES" << "\n";
        for(int i = 0; i < N_WORKERS; i++) {
            cout << "============================================" << "\n";
            cout << "WORKER " << i << "\n";
            cout << "Número de paquetes procesados: " << procesados[i] << "\n";
            cout << "Tiempo medio de procesado: " << MediaV[i] << "\n";
            cout << "Tiempo minimo de procesado: " << MinV[i] << "\n";
            cout << "Tiempo maximo de procesado: " << MaxV[i] << "\n";
            cout << "Porcentaje de paquetes procesados respecto al total: " << procesados[i]/float(nDatos) << "\n";
            cout << "Diferencia con el mejor tiempo: " << MinV[i] - Min << "\n";
            cout << "Diferencia con el peor tiempo: " << Max - MaxV[i] << "\n";
            cout << "Diferencia con la media: " << Media - MediaV[i] << endl;
        }
        cout << "============================================" << "\n";
        cout << "BYE BYE" << endl;
     }
    else {
        cout << "Ejecutar de la siguiente forma:" << endl;
        cout << "./analizadorTags XXXX YYYYYYYYY" << endl;
        cout << "siendo XXXX el número del puerto a emplear para comunicarse con gestor de colas" << endl;
        cout << "siendo YYYYYYYYY la ip a emplear para comunicarse con gestor de colas" << endl;
    }
}