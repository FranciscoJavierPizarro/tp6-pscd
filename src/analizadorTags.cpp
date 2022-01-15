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
#include "Semaphore_V4/Semaphore_V4.hpp"
#include <thread>
#include <ctime>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;
#include "arbin/arbin.h"
#include "arbin/pila.h"
#include "arbin/inform.h"
#include <iomanip>

void tagsInformation(Semaphore& imprimir, int nTotal, int nTotalTags) {
    ifstream f;
    f.open("./../src/tempFiles/tags.txt");
    string texto;
    inform p;
    arbin<inform> c;
    crear(c);
    if(f.is_open()) { //leemos del fichero
        getline(f,texto);
        while (!f.eof()) {
            crear(texto,1,p);
            anyadir(p,c);
            getline(f,texto);
        }
    }
    inform vecp[10];
    iniciarIterador(c);
    siguiente(c, p);
    vecp[0] = p;
    for(int i = 1; i < 9; i++) { // inicializamos vector para filtrar los 10 más usados
        siguiente(c, p);
        bool encontrado = false;
        for (int j = i-1; j >= 0 && !encontrado ;j--) {
            if (identificador(p) == identificador(vecp[j])) {
                encontrado = true;
            }
        }
        if (!encontrado) {
            vecp[i] = p;
        }
    }
    iniciarIterador(c);
    int l = 0;
    bool repe = false;
    while (existeSiguiente(c)) {// procesamos los datos almacenados para quedarnos con los 10 más usados
        siguiente(c, p);
        l = 0;
        repe = false;
        while(!(p > vecp[l]) && l < 10 && !repe) {
            if (identificador(p) == identificador(vecp[l])) repe = true;
            l++;
        }
        if(l < 9) {
            for(int i = 9; i >= l && !repe; i--) {
                if(i == l) vecp[l] = p;
                else vecp[i] = vecp[i - 1];
            }
        }       
    }
    imprimir.wait();
    cout << "\n+----------------------\033[1;4mTOP 10 HASHTAGS MÁS USADOS\033[0m----------------------+\n";
    for(int i = 0; i <= 9; i++) {
        cout << "\tHASHTAG Nº " + to_string(i+1) + ": " + identificador(vecp[i]) << endl;
        cout << "\tNÚMERO DE APARICIONES: " << valor(vecp[i]) << endl;
        cout << "\tPORCENTAJE HASHTAG/NÚMERO DE TWEETS EN EL SISTEMA: " << valor(vecp[i])/float(nTotal)*100 << "%" << endl;
        cout << "\tPORCENTAJE HASHTAG/NÚMERO DE HASHTAGS TOTALES EN EL SISTEMA: " << valor(vecp[i])/float(nTotalTags)*100 << "%" << endl;
    }
    cout << "+--------------------------------------------------------------------------------------+\n";
    imprimir.signal();
}

void authorsInformation(Semaphore& imprimir) {
    ifstream f;
    f.open("./../src/tempFiles/authors.txt");
    string texto;
    inform p;
    arbin<inform> c;
    crear(c);
    if(f.is_open()) {//leemos del fichero
        getline(f,texto);
        while (!f.eof()) {
            crear(texto,1,p);
            anyadir(p,c);
            getline(f,texto);
        }
    }
    inform vecp[10];
    iniciarIterador(c);
    siguiente(c, p);
    vecp[0] = p;
    for(int i = 1; i < 9; i++) {// inicializamos vector para filtrar los 10 más usados
        siguiente(c, p);
        bool encontrado = false;
        for (int j = i-1; j >= 0 && !encontrado ;j--) {
            if (identificador(p) == identificador(vecp[j])) {
                encontrado = true;
            }
        }
        if (!encontrado) {
            vecp[i] = p;
        }
    }
    iniciarIterador(c);
    int l = 0;
    bool repe = false;
    while (existeSiguiente(c)) {// procesamos los datos almacenados para quedarnos con los 10 más usados
        siguiente(c, p);
        l = 0;
        repe = false;
        while(!(p > vecp[l]) && l < 10 && !repe) {
            if (identificador(p) == identificador(vecp[l])) repe = true;
            l++;
        }
        if(l < 9) {
            for(int i = 9; i >= l && !repe; i--) {
                if(i == l) vecp[l] = p;
                else vecp[i] = vecp[i - 1];
            }
        }       
    }
    imprimir.wait();
    cout << "\n+-----------------------------\033[1;4mTOP 10 AUTORES MÁS ACTIVOS\033[0m-----------------------------+\n";
    for(int i = 0; i <= 9; i++) {
        cout << "\tAUTOR Nº " + to_string(i+1) + ": " + identificador(vecp[i]) << endl;
        cout << "\tACTIVIDAD POR LA RED (tweets,retweets...): " << valor(vecp[i]) << endl;
    }
    cout << "+----------------------------------------------------------------------------------------------------+\n";
    imprimir.signal();
}

void mencionInformation(Semaphore& imprimir, int nTotal, int nMenciones) {
    ifstream f;
    f.open("./../src/tempFiles/mencion.txt");
    string texto;
    inform p;
    arbin<inform> c;
    crear(c);
    if(f.is_open()) {//leemos del fichero
        getline(f,texto);
        while (!f.eof()) {
            crear(texto,1,p);
            anyadir(p,c);
            getline(f,texto);
        }
    }
    inform vecp[10];
    iniciarIterador(c);
    siguiente(c, p);
    vecp[0] = p;
    for(int i = 1; i < 9; i++) {// inicializamos vector para filtrar los 10 más usados
        siguiente(c, p);
        bool encontrado = false;
        for (int j = i-1; j >= 0 && !encontrado ;j--) {
            if (identificador(p) == identificador(vecp[j])) {
                encontrado = true;
            }
        }
        if (!encontrado) {
            vecp[i] = p;
        }
    }
    iniciarIterador(c);
    int l = 0;
    bool repe = false;
    while (existeSiguiente(c)) {// procesamos los datos almacenados para quedarnos con los 10 más usados
        siguiente(c, p);
        l = 0;
        repe = false;
        while(!(p > vecp[l]) && l < 10 && !repe) {
            if (identificador(p) == identificador(vecp[l])) repe = true;
            l++;
        }
        if(l < 9) {
            for(int i = 9; i >= l && !repe; i--) {
                if(i == l) vecp[l] = p;
                else vecp[i] = vecp[i - 1];
            }
        }       
    }
    imprimir.wait();
    cout << "\n+----------------------\033[1;4mTOP 10 AUTORES MAS INFLUYENTES\033[0m----------------------+\n";
    for(int i = 0; i <= 9; i++) {
        cout << "\tAUTOR Nº " + to_string(i+1) + ": " + identificador(vecp[i]) << endl;
        cout << "\tNÚMERO DE MENCIONES: " << valor(vecp[i]) << endl;
        cout << "\tPORCENTAJE MENCIONES/NÚMERO DE TWEETS EN EL SISTEMA (influencia): " << valor(vecp[i])/float(nTotal)*100 << "%" << endl;
        cout << "\tPORCENTAJE MENCIONES/NÚMERO DE MENCIONES TOTALES EN EL SISTEMA: " << valor(vecp[i])/float(nMenciones)*100 << "%" << endl;
    }
    cout << "+------------------------------------------------------------------------------------------+\n";
    imprimir.signal();
}

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
            cerr << "error: no se ha realizado la conexión\n";
            exit(1);
        }
        cout << "\033[32;1;4mCONEXION ESTABLECIDA\033[0m" << endl;

        // declaración de variables para intercambio de mensajes:
        const string MESSAGE = "READ_TAGS";
        string respuesta,aux;
        int LENGTH = 500;
        int send_bytes,len,n,read_bytes;
        bool finDatos = false;
        int a,b;
        //VARIABLES PARA PROCESADO
        int webApp = 0,iphone = 0,android = 0,wordpress = 0, misc = 0;
        int webAppRT = 0,iphoneRT = 0,androidRT = 0,wordpressRT = 0, miscRT = 0;
        int webAppO = 0,iphoneO = 0,androidO = 0,wordpressO = 0, miscO = 0;
        int webAppH = 0,iphoneH = 0,androidH = 0,wordpressH = 0, miscH = 0;
        int webAppM = 0,iphoneM = 0,androidM = 0,wordpressM = 0, miscM = 0;
        ofstream tags("./../src/tempFiles/tags.txt");
        ofstream authors("./../src/tempFiles/authors.txt");
        ofstream mencion("./../src/tempFiles/mencion.txt");
        string auxProcesado;
        if(tags.is_open() && authors.is_open() && mencion.is_open()) {
            while (!finDatos) {
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
                cout << "PETICIÓN ENVIADA" << endl;

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

                if (respuesta == MENS_FIN) { // si llega mensaje de fin, paramos el bucle
                    cout << "MENSAJE DE FIN RECIBIDO\n";
                    finDatos = true;
                    // Cerramos el socket
                    int error_code = chanGestor.Close(socket_fd_gestor);
                    if(error_code == -1) {
                        cerr << "Error cerrando el socket: " << strerror(errno) << endl;
                    }
                    cout << "\033[32;1;4mCONEXION FINALIZADA\033[0m" << endl;
                }
                else { 
                    cout << "DATOS RECIBIDOS" << endl;
                    //PROCESAR MENSAJE
                    a = respuesta.find("$0 ") + 3;
                    b = respuesta.find_first_of("/",a);
                    webApp += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    iphone += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    android += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    wordpress += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    misc += stoi(respuesta.substr(a,b - a));

                    a = respuesta.find("$1 ") + 3;
                    b = respuesta.find_first_of("/",a)- a;
                    webAppRT += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    iphoneRT += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    androidRT += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    wordpressRT += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    miscRT += stoi(respuesta.substr(a,b - a));

                    a = respuesta.find("$2 ") + 3;
                    b = respuesta.find_first_of("/",a);
                    webAppO += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    iphoneO += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    androidO += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    wordpressO += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    miscO += stoi(respuesta.substr(a,b - a));

                    a = respuesta.find("$3 ") + 3;
                    b = respuesta.find_first_of("/",a);
                    webAppH += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    iphoneH += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    androidH += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    wordpressH += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    miscH += stoi(respuesta.substr(a,b - a));

                    a = respuesta.find("$4 ") + 3;
                    b = respuesta.find_first_of("/",a);
                    webAppM += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    iphoneM += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    androidM += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    wordpressM += stoi(respuesta.substr(a,b - a));
                    a = respuesta.find_first_of("/",b) + 1;
                    b = respuesta.find_first_of("/",a);
                    miscM += stoi(respuesta.substr(a,b - a));
                    
                    a = respuesta.find("$5 ") + 3;
                    b = respuesta.find_first_of("/", a);
                    auxProcesado = respuesta.substr(a, b - a);

                    a = 0;
                    b = auxProcesado.find_first_of(";", a);
                    while(b != a) {
                        authors << auxProcesado.substr(a,b - a) + "\n";
                        a = b;
                        b = auxProcesado.find_first_of(";", a);
                    }
                    
                
                    a = respuesta.find("$6 ") + 3;
                    b = respuesta.find_first_of("%", a);
                    auxProcesado = respuesta.substr(a, b - a);
                    a = 0;
                    b = auxProcesado.find_first_of("#", a + 1);
                    while(b != a) {
                        tags << auxProcesado.substr(a,b - a) + "\n";
                        a = b;
                        b = auxProcesado.find_first_of("#", a);
                    }
                    
                    a = respuesta.find("$7 ") + 3;
                    b = respuesta.find_first_of("%", a);
                    auxProcesado = respuesta.substr(a, b - a);

                    if(auxProcesado != "") { // si no hay menciones no procesamos
                        a = 0;
                        b = auxProcesado.find_first_of("@", a + 1);
                        while(b != a) {
                            mencion << auxProcesado.substr(a,b - a) + "\n";
                            a = b;
                            b = auxProcesado.find_first_of("@", a);
                        }
                    }
                    
                }
            }
        }
        tags.close();
        authors.close();
        mencion.close();
        
        int nTotal, nTotalTags, nMenciones;
        nTotal = webApp + iphone + android + wordpress + misc;
        nTotalTags = webAppH + iphoneH + androidH + wordpressH + miscH;
        nMenciones = webAppM + iphoneM + androidM + wordpressM + miscM;
        Semaphore imprimir(1);
        thread Ttags, Tauthors, Tmencion;
        Ttags = thread(&tagsInformation, ref(imprimir), nTotal, nTotalTags);
        Tauthors = thread(&authorsInformation, ref(imprimir));
        Tmencion = thread(&mencionInformation, ref(imprimir), nTotal, nMenciones);

        cout << "\n+--------------------------\033[1;4mESTADISTICAS NUMÉRICAS\033[0m----------------------------+\n";
        cout << "CLIENTE             TOTAL     RT     ORIGINAL  TAGS   MENCIONES  PORCENTAJE\n";
        cout << "WebApp:        " << setw(8) << webApp << " " << setw(8) << webAppRT << " " << setw(8) << webAppO << " " << setw(8) << webAppH << " " << setw(8) << webAppM << " " << setw(14) << webApp/float(nTotal) << "\n"; 
        cout << "Iphone:        " << setw(8) << iphone << " " << setw(8) << iphoneRT << " " << setw(8) << iphoneO << " " << setw(8) << iphoneH << " " << setw(8) << iphoneM << " " << setw(14) << iphone/float(nTotal) << "\n";
        cout << "Android:       " << setw(8) << android << " " << setw(8) << androidRT << " " << setw(8) << androidO << " " << setw(8) << androidH << " " << setw(8) << androidM << " " << setw(14) << android/float(nTotal) << "\n";
        cout << "Wordpress:     " << setw(8) << wordpress << " " << setw(8) << wordpressRT << " " << setw(8) << wordpressO << " " << setw(8) << wordpressH << " " << setw(8) << wordpressM << " " << setw(14) << wordpress/float(nTotal) << "\n";
        cout << "Otros:         " << setw(8) << misc << " " << setw(8) << miscRT << " " << setw(8) << miscO << " " << setw(8) << miscH << " " << setw(8) << miscM << " " << setw(14) << misc/float(nTotal) << "\n";
        cout << "+--------------------------------------------------------------------------+\n";

        imprimir.wait();
        cout << endl;
        imprimir.signal();
        Ttags.join();
        Tauthors.join();
        Tmencion.join();
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
