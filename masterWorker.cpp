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
/*
Current state:
Protocolo de comunicaciones hecho
Procesamiento de las respuestas master hecho
Falta concretar/implementar que deben enviar los workers
y comentar el código


*/

void master(int PORT_STREAMING, string IP_STREAMING, int PORT_GESTOR, string IP_GESTOR) {
    string MENS_FIN = "FIN";
    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chanGestor(IP_GESTOR, PORT_GESTOR);

    // Conectamos con el servidor. Probamos varias conexiones
    const int MAX_ATTEMPS = 10;
    int count = 0;
    
    Socket chanStream(IP_STREAMING, PORT_STREAMING);

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
    // Conectamos con el servidor. Probamos varias conexiones
    count = 0;

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
    cout << "CONEXIONES ESTABLECIDAS" << endl;
    int LENGTH = 10000;
    string tareas[5];
    string mensaje;
    int read_bytes;   //num de bytes recibidos en un mensaje
    int send_bytes;  //num de bytes enviados en un mensaje

    for(int j = 0; j < 3; j++) {
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
    // Enviamos el mensaje de fin al servicio de streaming
    send_bytes = chanStream.Send(socket_fd_streaming, MENS_FIN);
            
    if(send_bytes == -1) {
        cerr << "Error al enviar datos: " << strerror(errno) << endl;
        // Cerramos el socket
        chanStream.Close(socket_fd_streaming);
        exit(1);
    }
    // Enviamos el mensaje de fin al servicio gestor de colas
    send_bytes = chanGestor.Send(socket_fd_gestor, MENS_FIN);
            
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
    cout << "CONEXIONES FINALIZADAS" << endl;
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
    cout << "CONEXION ESTABLECIDA" << endl;
    int LENGTH = 10000;
    string tweets[5];
    string mensaje = "";
    string perf, tags;
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
            proccessTaskBlock(mensaje,perf,tags);
            // Enviamos el mensaje de petición al gestor
            mensaje = "PUBLISH_QoS," + perf + "," + to_string(id);
            send_bytes = chanGestor.Send(socket_fd_gestor, mensaje);
                
            if(send_bytes == -1) {
                cerr << "Error al enviar datos: " << strerror(errno) << endl;
                // Cerramos el socket
                chanGestor.Close(socket_fd_gestor);
                exit(1);
            }
            // Enviamos el mensaje de petición al gestor
            mensaje = "PUBLISH_TAGS,"+ tags + "," + to_string(id);
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
    cout << "CONEXION FINALIZADA" << endl;
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
        mast = thread(&master, PORT_STREAMING, IP_STREAMING, PORT_GESTOR, IP_GESTOR);
        for(int i = 0; i < N_WORKERS; i++) {
            workers[i] = thread(&worker,PORT_GESTOR, IP_GESTOR, i);
        }
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
    // string perf, tags;
    // string pruebas = "$0 2021-10-14 19:04:12;Twitter Web App;x_y_es;RT: 11/10/2021 He mejorado bastante el vuelo #3D y ahora muestra la isla completa, y las 6 últimas coladas juntas días 6 al 11. Con datos del @CabLaPalma sobre un mapa del @IGNSpain. @lapalmaopendata @InnovaLaPalma #VigilanciaLaPalma #LaPalma #ErupciónLaPalma #VolcandeLaPalma https://t.co/3xLFCqIqCY$1 2021-10-14 19:03:31;Twitter Web App;chematierra;RT: #ErupcionLaPalma Octubre 14, continúa la erupción con fuertes rugidos y ríos de lava en #CumbreVieja #LaPalma #Canarias #España  Al día de ayer: superficie cubierta 680 Ha casas destruídas 1548 casas dañadas 86 res carretera afectada 53km Créditos  @involcan https://t.co/2BrO7pydqR$2 2021-10-14 19:03:29;Twitter for Android;IGNSpain;RT:  Video de ayer del paso del río de lava desde la ladera norte del #VolcandeLaPalma. #IGNSpain #VolcanLaPalma #ErupcionLaPalma @mitmagob @CabLaPalma @DgCanarias @IGME1849 @IGeociencias @VolcansCanarias @RTVCes https://t.co/naisBgSNR6$3 2021-10-14 19:03:20;Twitter for Android;RTVCes;RT: #erupciónLaPalma  Imágenes de las 14:15 grabadas por @Involcan en las que se registra el desborde de la colada de lava en el cono principal del #volcán en #LaPalma https://t.co/nJ1bnfQyUs$4 2021-10-14 19:03:05;Twitter Web App;i_ameztoy;RT: #LaPalma   Situación a 13 de octubre a las 06:50 UTC y evolución desde el principio del evento; La colada en la zona Norte sigue su curso  Datos de @CopernicusEMS y mapa base de @IGNSpain HD: https://t.co/OprlPNwTui  #ErupciónLaPalma #volcanCumbreVieja #LaPalmaEruption https://t.co/CsikEFJ1Xi$5";
    // proccessTaskBlock(pruebas,perf,tags);
    // cout << perf << endl;
    // cout << tags << endl;
    return 0;
}