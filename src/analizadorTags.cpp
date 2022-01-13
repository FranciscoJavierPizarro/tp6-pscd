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
#include "nodoTag.h"
#include "nodoUsuario.h"
#include "listaGenerica.h"
#include <thread>
#include <ctime>
#include <string>
using namespace std;

// VARIABLES GLOBALES
const int BUFFSIZE = 20;
string buffer[BUFFSIZE];
int size = 0;
// constantes para la gestión de procesamiento de los resultados del gestor de colas:
const char delim = '/';
const char delimAutor = '&';
const char delimHashtag = '#';
const char delimTag = '%';

/* en el nodo, hay un cliente, número de usos totales del cliente con el que se accede a twitter,
 * con el que se escriben twteets, con el que se escriben tweets con tags, con el que se
 * escriben tweets con menciones y con el que se hace retweet.
 */
struct nodoRG {
    string Client = string();
    int usoClient = 0;
    int usoTweetC = 0;
    int usoTTagC = 0;
    int usoTMencionC = 0;
    int usoRTC = 0;
};
/* Clasificamos los clientes como los 4 más usados según una extracción
 * de tweets previa al lanzamiento del sistema y un cliente que servirá de comodín para el resto.
 * inicialmente tendrá la siguiente disposición: 
 * (al finalizar la información a procesar se ordenará de más usado al menos)
 * listaGlobales[0] es webApp
 * listaGlobales[1] es iphone
 * listaGlobales[2] es android
 * listaGlobales[3] es wordPress
 * listaGlobales[4] es el resto
 */
nodoRG listaGlobales[5];

/* Dado un Hashtag: hay un nodoUser, especificado en nodoUsuario.h*/
struct resultadosHashtags {
    string Hashtag;
    lista<nodoUser> autores;
};

resultadosHashtags top10[10];

// lista utilizada para almacenar los resultados de los tags
lista<nodoRT> listaTags;

//lista utilizada para almacenar los resultados parciales de los tags del buffer en cada reescritura del buffer
lista<nodoRT> listaTagsAux;

// GESTIÓN SINCRONIZACIÓN:
Semaphore heTerminado(0);
Semaphore sigoTags(0);
Semaphore sigoGlobales(0);
bool finDatos = false;

bool finBloques = false;
bool ultimaIteracion = false;
Semaphore hechoBloque(0);
Semaphore sigoTRT(0);
Semaphore sigoTO(0);
Semaphore sigoTM(0);
string tagsRT,tagsO,tagsM;

/********************************** FUNCIONES AUXILIARES ***********************************/
// Pre: soc tiene establecida una conexión con el gestor de colas y debe haber suficientes datos como para llenar el buffer
// Post: devuelve el buffer inicializado con los primeros datos a procesar
void inicializarBuffer(Socket& soc,int socket_fd_gestor, const string mensaje, const int leng) {
    cout << "-----------------\nINICIALIZANDO VECTOR\n-----------------\n";
    string respuesta,aux;
    int read_bytes,send_bytes;
    int len = mensaje.length();
    int n = len/500;
    for(int k = 0; k < 25; k++) {
        if(k <  n) send_bytes = soc.Send(socket_fd_gestor, mensaje.substr(k*500,500));
        else if(k == n) send_bytes = soc.Send(socket_fd_gestor, mensaje.substr(k*500,500)+"$$");
        else send_bytes = soc.Send(socket_fd_gestor, " ");
        if(send_bytes == -1) {
            cerr << "Error al enviar datos al gestor: " << strerror(errno) << endl;
            // Cerramos el socket
            soc.Close(socket_fd_gestor);
            exit(1);
        }
    }
    for (int i = 0; i < BUFFSIZE; i++) {
        respuesta = "";
        for(int i = 0; i < 25; i++) {
            // Recibimos la respuesta del servidor gestor
            read_bytes = soc.Recv(socket_fd_gestor, aux, leng);
                
            if(read_bytes == -1) {
                cerr << "Error al recibir datos: " << strerror(errno) << endl;
                // Cerramos el socket
                soc.Close(socket_fd_gestor);
                exit(1);
            }
            respuesta.append(aux);
        }
        respuesta = respuesta.substr(0,respuesta.find("$$"));

        buffer[i] = respuesta;
        for(int k = 0; k < 25; k++) {
            if(k <  n) send_bytes = soc.Send(socket_fd_gestor, mensaje.substr(k*500,500));
            else if(k == n) send_bytes = soc.Send(socket_fd_gestor, mensaje.substr(k*500,500)+"$$");
            else send_bytes = soc.Send(socket_fd_gestor, " ");
            if(send_bytes == -1) {
                cerr << "Error al enviar datos al gestor: " << strerror(errno) << endl;
                // Cerramos el socket
                soc.Close(socket_fd_gestor);
                exit(1);
            }
        }
    }
}

// Pre: buffer es un string con la estructura del mensaje depositado en la cola de resultados:
//      buffer = $0 + información de resultados + $1 + info + ... + $5 + info
//      infoDeseada > 0 AND infoDeseada <= 5
// Post: Devuelve el string $(infoDeseada) + información
string obtenerTokenLista(const int infoDeseada,const string buffer) {
    int posIn,posFin;
    string token;
    posIn = buffer.find("$" + to_string(infoDeseada),0) + 2; // el 2 es para saltarnos el caracter "$" e infoDeseada
    if (infoDeseada == 6) { // caso especial ya que es el final
        token = buffer.substr(posIn,buffer.length()-posIn);
    }
    else {
        posFin = buffer.find_first_of('$',posIn);
        token = buffer.substr(posIn,posFin-posIn);
    }
    return token;
}

// Pre: posIn > 0 AND posIn < #buffer 
// Post: inRange = posIn < #buffer
//       resultado = buffer[posIn] + buffer[posIn+1] + ... + buffer[i-1] siendo buffer[i] = delimitador o buffer[#buffer-1]
//       AND posIn = i + 1
//       AND (posIn >= #buffer -> posIn = 0)
void obtenerToken(int& posIn,const string buffer,const char delimitador, string& resultado,bool& inRange) {
    inRange = true;
    if (posIn >= buffer.length()) {
        inRange = false;
        posIn = 0;
    }
    else {
        int i = buffer.find_first_of(delimitador,posIn);
        if (i == string::npos) { // es el final del string
            i = buffer.length();
        }
        resultado = buffer.substr(posIn,i-posIn);
        posIn = i + 1;
    }
}

// Pre: hashtag es un hashtags válido dentro de las normas de twitter.
// Post: estandariza el hashtag según las reglas de twitter:
//       no hay diferencias entre mayúscula y minúscula (se pondrá todo minúsculas)
//       tampoco entre llevar tilde o no (se pondrá sin tilde)
//       ni entre llevar ñ o n (se pondrá con n)
void estandarizarHashtag(string& hashtag) {
    for (int i = 0; i < hashtag.length(); i++) {
        if (isalpha(hashtag[i])) {
            tolower(hashtag[i]);
        }
        if (hashtag[i] == 'á') {
            hashtag[i] = 'a';
        }
        else if (hashtag[i] == 'é') {
            hashtag[i] = 'e';
        }
        else if (hashtag[i] == 'í') {
            hashtag[i] = 'i';
        }
        else if (hashtag[i] == 'ó') {
            hashtag[i] = 'o';
        }
        else if (hashtag[i] == 'ú') {
            hashtag[i] = 'u';
        }
        else if (hashtag[i] == 'ñ') {
            hashtag[i] ='n';
        }
    }
}

// Pre: Ordenar es un vector de structs de tipo nodoRG
// Post: Ordenar contiene los structs ordenados de mayor tamaño de la
//      componente usoClient a menor tamaño

void ordenacion(nodoRG ordenar[], int limite) {
    nodoRG aux;
    for (int i = 0; i < limite - 1; i++) {
        for (int j = 1; j <= limite; j++) {
            if (ordenar[i].usoClient < ordenar[j].usoClient) {
                aux = ordenar[i];
                ordenar[i] = ordenar[j];
                ordenar[j] = aux;
            }
        }
    }
}

/********************************** HILOS (Procesos) del analizadorTags: ***********************************/

// Pre: ---
// Post: mientras no hay más datos que leer,lee del buffer, procesa cada elemento hasta size, y el resultado 
//       (descrito en la declaración de su lista) lo almacena en listaGlobales. 
void globalAnalyzer() {
    // variables
    bool fin = false;
    while(!fin) {
        for (int i = 0; i < size; i++) {
            string aux = buffer[i];
            string usoAuxiliar = obtenerTokenLista(0,aux);
            int puntero = 0;
            string usos;
            char delimitador = '/';
            bool hayToken;

            for (int j = 0; j <= 4; j++) {
                obtenerToken(puntero, usoAuxiliar, delimitador, usos, hayToken);
                listaGlobales[j].usoClient = listaGlobales[j].usoClient + stoi(usos);
            }
            
            for (int j = 0; j <= 4; j++) {
                obtenerToken(puntero, usoAuxiliar, delimitador, usos, hayToken);
                listaGlobales[j].usoRTC = listaGlobales[j].usoRTC + stoi(usos);
            }
            
            for (int j = 0; j <= 4; j++) {
                obtenerToken(puntero, usoAuxiliar, delimitador, usos, hayToken);
                listaGlobales[j].usoTweetC = listaGlobales[j].usoTweetC + stoi(usos);
            }

            for (int j = 0; j <= 4; j++) {
                obtenerToken(puntero, usoAuxiliar, delimitador, usos, hayToken);
                listaGlobales[j].usoTTagC = listaGlobales[j].usoTTagC + stoi(usos);
            }

            for (int j = 0; j <= 4; j++) {
                obtenerToken(puntero, usoAuxiliar, delimitador, usos, hayToken);
                listaGlobales[j].usoTMencionC = listaGlobales[j].usoTMencionC + stoi(usos);
            }
		}
        heTerminado.signal();
        if (finDatos) {
            fin = true;
        }
        sigoGlobales.wait();
    }
    ordenacion(listaGlobales,4);
}

void tagsAnalyzerMain() {
    bool fin = false;
    int tagPointer,hashtagPointer,indAux;
    string tagsAux,tokenTags,hashtag,autor;
    bool inRange; 
    nodoRT* auxP;
    nodoRT auxNodo;
    while (!fin) {
        for (int i = 0; i < size; i++) {
            // inicializando strings con los que trabajarán los threads auxiliares del analizador de tagsAnalyzer
            tagsRT = string();
            tagsO = string();
            tagsM = string();
            tokenTags = obtenerTokenLista(5,buffer[i]);
            tagsAux = string();
            obtenerToken(tagPointer,tokenTags,delimTag,tagsAux,inRange); // deja solo tags en tagsAux
            if (inRange) { // si es vacío no procesamos tags
                obtenerToken(tagPointer,tokenTags,delimTag,tagsRT,inRange); // deja los tags en retweets en tagsRT
                obtenerToken(tagPointer,tokenTags,delimTag,tagsO,inRange); // deja los tags en tweets en tagsO
                obtenerToken(tagPointer,tokenTags,delimTag,tagsM,inRange); // deja los tags en tweets con menciones en tagsO
                // PROCESAMIENTO DE TAGS
                hashtagPointer = 1; // ignoramos el primer "#"
                auxP = nullptr;
                obtenerToken(hashtagPointer,tagsAux,delimAutor,hashtag,inRange); // obtenemos el hashtag
                obtenerToken(hashtagPointer,tagsAux,delimHashtag,autor,inRange); // obtenemos el autor
                while (inRange) {
                    estandarizarHashtag(hashtag);
                    if (obtenerNodo(listaTagsAux,hashtag,auxP) > 0) {
                        actTotal(*auxP,1);
                        actQuienTotal(*auxP,autor);
                    }
                    else { // no está en la lista
                        iniciarNodoTag(auxNodo,hashtag);
                        insertarNodo(listaTagsAux,auxNodo);
                    }
                    obtenerToken(hashtagPointer,tagsAux,delimAutor,hashtag,inRange);
                    obtenerToken(hashtagPointer,tagsAux,delimHashtag,autor,inRange); 
                }
            }
            if ((i == size-1) && ultimaIteracion) {
                finBloques = true;
            }
            sigoTRT.signal();
            sigoTO.signal();
            sigoTM.signal();
            hechoBloque.wait(3);
        }
        // inserción en listaTags
        auxP = nullptr;
        iniciarIterador(listaTagsAux);
        while (existeSiguiente(listaTagsAux)) {
            auxNodo = siguiente(listaTagsAux);
            if (indAux = obtenerNodo(listaTags,identificador(auxNodo),auxP) > 0) {
                actApHashtagM(*auxP,mostrarHashtagM(auxNodo));
                actApHashtagRT(*auxP,mostrarHashtagRT(auxNodo));
                actApHashtagT(*auxP,mostrarHashtagT(auxNodo));
                actTotal(*auxP,total(auxNodo));
                actQuienHashtagM(*auxP,mostrarQuienHM(auxNodo));
                actQuienHashtagRT(*auxP,mostrarQuienHRT(auxNodo));
                actQuienHashtagT(*auxP,mostrarQuienHM(auxNodo));
                actTotal(*auxP,total(auxNodo));
                // actualizamos el nodo en listaTags
                actualizarNodo(listaTags,indAux);
            }
            else {
                insertarNodo(listaTags,auxNodo);
            }
        }
        // borramos la lista auxiliar
        vaciarLista(listaTags);
        
        heTerminado.signal();
        if (finDatos) {
            fin = true;
            ultimaIteracion = true;
        }
    }
}

void tagsAnalyzerRT() {
    bool fin = false;
    int rtPointer;
    int hashtagPointer; 
    bool inRange;
    nodoRT* auxP;
    string tweetAux,hashtag,autor;
    while (!fin) {
        sigoTRT.wait();
        rtPointer = 0;
        hashtagPointer = 1; // ignoramos el primer "#"
        auxP = nullptr;
        tweetAux = string();
        // PROCESAMIENTO DE TAGS_RT
        obtenerToken(rtPointer,tagsRT,delim,tweetAux,inRange); // deja solo rt en tweetAux
        while (inRange) {
            obtenerToken(hashtagPointer,tweetAux,delimAutor,hashtag,inRange); // obtenemos el hashtag
            obtenerToken(hashtagPointer,tweetAux,delimHashtag,autor,inRange); // obtenemos el autor
            while (inRange) {
                estandarizarHashtag(hashtag);
                if (obtenerNodo(listaTagsAux,hashtag,auxP)) {
                    actApHashtagRT(*auxP,1);
                    actQuienHashtagRT(*auxP,autor);
                }
                obtenerToken(hashtagPointer,tweetAux,delimAutor,hashtag,inRange);
                obtenerToken(hashtagPointer,tweetAux,delimHashtag,autor,inRange); 
            }
            obtenerToken(rtPointer,tagsRT,delim,tweetAux,inRange); // deja solo rt en tweetAux
        }
        hechoBloque.signal();
        if (finBloques) {
            fin = true;
        }
    }
}

void tagsAnalyzerO() {
    bool fin = false;
    int tweetOPointer;
    int hashtagPointer;
    bool inRange;
    nodoRT* auxP;
    string tweetAux,hashtag,autor;
    while (!fin) {
        sigoTO.wait();
        tweetOPointer = 0;
        hashtagPointer = 1; // ignoramos el primer "#"
        auxP = nullptr;
        tweetAux = string();
        // PROCESAMIENTO DE TAGS_O
        obtenerToken(tweetOPointer,tagsO,delim,tweetAux,inRange); // deja solo rt en tweetAux
        while (inRange) {
            obtenerToken(hashtagPointer,tweetAux,delimAutor,hashtag,inRange); // obtenemos el hashtag
            obtenerToken(hashtagPointer,tweetAux,delimHashtag,autor,inRange); // obtenemos el autor
            while (inRange) {
                estandarizarHashtag(hashtag);
                if (obtenerNodo(listaTagsAux,hashtag,auxP)) {
                    actApHashtagT(*auxP,1);
                    actQuienHashtagT(*auxP,autor);
                }
                obtenerToken(hashtagPointer,tweetAux,delimAutor,hashtag,inRange);
                obtenerToken(hashtagPointer,tweetAux,delimHashtag,autor,inRange); 
            }
            obtenerToken(tweetOPointer,tagsO,delim,tweetAux,inRange); // deja solo rt en tweetAux
        }
        hechoBloque.signal();
        if (finBloques) {
            fin = true;
        }
    }
}

void tagsAnalyzerM() {
    bool fin = false;
    int tweetMPointer;
    int hashtagPointer;
    int hP;
    nodoRT* auxP;
    string tweetAux,hashtags,hashtag,autor,menciones,autorMenciones;
    bool inRange;
    while (!fin) {
        sigoTM.wait();
        tweetMPointer = 0;
        hashtagPointer = 1; // ignoramos el primer "#"
        hP = 0;
        auxP = nullptr;
        tweetAux = string();
        // PROCESAMIENTO DE TAGS_M
        obtenerToken(tweetMPointer,tagsM,delim,tweetAux,inRange); // deja solo tweetM en tweetAux
        while (inRange) {
            obtenerToken(hashtagPointer,tweetAux,delimHashtag,menciones,inRange); // obtenemos las menciones
            obtenerToken(hashtagPointer,tweetAux,delimAutor,hashtags,inRange); // obtenemos los hashtags
            obtenerToken(hashtagPointer,tweetAux,delimHashtag,autor,inRange); // obtenemos el autor (delimHashtag se pone por defecto)
            // PROCESAMIENTO DE HASHTAG DE HASHTAGS
            autorMenciones = autor + "&" + menciones;
            obtenerToken(hP,hashtags,delimHashtag,hashtag,inRange); // obtenemos el hashtag de los hashtags
            while (inRange) {
                estandarizarHashtag(hashtag);
                if (obtenerNodo(listaTagsAux,hashtag,auxP)) {
                    actApHashtagM(*auxP,1);
                    actQuienHashtagM(*auxP,autorMenciones);
                }
                obtenerToken(hP,hashtags,delimHashtag,hashtag,inRange);
            }
            obtenerToken(tweetMPointer,tagsM,delim,tweetAux,inRange); // deja solo tweetM en tweetAux
        }
        hechoBloque.signal();
        if (finBloques) {
            fin = true;
        }
    }
}

// Pre: ---
// Post: busca el nodo posHashtag de la lista listaTags y 
//       procesa cada elemento hasta size, y el resultado (descrito en la declaración de su lista)
//       lo la almacena en su lista resultadosUser[posHashtag]
void userAnalyzer(const int posHashtag) {
    /*  algoritmo crearListaPorTag():
        buscar resultadosUser[posHashtag]
        si posHashtag + 1 <= listaTags.totalHashtags 
            buscar nodoRT[posHashtag] 
            Dado listaTags[posHashtag]:
                crear lista auxiliar de usuarios
                mientras no se acabe quienAH
                    buscarAutorEnLista(autorDeLista,suMomento) (consume los autores del string)
                    si está 
                        nodoAuxiliarDelUsuario.actividad++
                    sino
                        crearNuevoNodo() 
                        inicializarNodoVacío()
                        autor = autorDeLista 
                        actividad = 1
                    fsi
                fmientras
                mientras no se acabe quienAHT
                    buscarAutorEnLista(autorDeLista,suMomento) (consume los autores y su momento del string)
                    nodoAuxiliarDelUsuario.usoTweet++
                fmientras
                mientras no se acabe quienAHRT
                    buscarAutorEnLista(autorDeLista,suMomento) (consume los autores y su momento del string)
                    nodoAuxiliarDelUsuario.usoRT++
                fmientras
                mientras no se acabe quienAHM
                    buscarAutorEnLista(autorDeLista,suMomento) (consume los autores del string)
                    nodoAuxiliarDelUsuario.usoTMencion++
                fmientras
                // INSERCIÓN EN LA LISTA
                para cada nodo de la lista auxiliar
                    buscarAutor()
                    si está
                        actualizar esteNodoUser(nodoAuxiliar)
                        si esteNodoUser.actividad > nodoUser[9]
                            ordenarLista()
                        sino
                            no modificamos la lista
                        fsi
                    sino 
                        si nodoAuxiliar.actividad > nodoUser[9]
                            ordenarLista(nodoAuxiliar)
                        sino
                            inserciónAlFinal()
                        fsi
                    fsi
                fpara
            fdado
        sino 
            no hacemos nada
        fsi
    */
}

// Pre: ---
/* Post: Lee las diferentes listas para mostrar por pantalla las siguientes conclusiones:
 *  - Visión global:
 *      · Top 5 Clientes más utilizados para acceder a twitter
 *      · Top 5 Clientes más utilizados para escribir tweets
 *      · Top 5 Clientes más utilizados al escribir tweets con tags
 *      · Top 5 Clientes más utilizados al escribir tweets con menciones
 *      · Top 5 Clientes más utilizados para hacer retweet
 *  - Visión desde los tags:
 *      · Top 10 Hashtags más utilizados durante la vida del sistema
 *      · Top 10 Hashtags más utilizados en tweets originales
 *      · Top 10 Hashtags más utilizados en retweets
 *      · Top 10 Hashtags más utilizados en tweets con menciones
 *  - Visión desde los usuarios (por cada tag del top 10):
 *      · Top 10 usuarios con más actividad con el tag
 *      · Top 10 usuarios con más tweets escritos con el tag
 *      · Top 10 usuarios con más retweets con el tag
 *      · Top 10 usuarios con más menciones en tweets con el tag
 */
void cliente() {

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
            cout << "no se ha realizado la conexión\n";
            exit(1);
        }
        cout << "CONEXION ESTABLECIDA" << endl;

        // // declaración de variables para intercambio de mensajes:
        const string MESSAGE = "READ_TAGS";
        string respuesta,aux;
        int LENGTH = 500;
        int send_bytes,len,n,read_bytes;
        // inicializarBuffer(chanGestor,socket_fd_gestor,MESSAGE,LENGTH);
        // /********************************** GESTIÓN HILOS ***********************************/
        // thread th_globalAnalyzer(&globalAnalyzer);
        // // thread th_tagsAnalyzer(&tagsAnalyzer);
        // thread th_tagsAnalyerMain(&tagsAnalyzerMain);
        // thread th_tagsAnalyzerRT(&tagsAnalyzerRT);
        // thread th_tagsAnalyzerO(&tagsAnalyzerO);
        // thread th_tagsAnalyzerM(&tagsAnalyzerM);
        // /************************************************************************************/
        while (!finDatos) {

        //     heTerminado.wait(2);






        // /* COMUNICACIÓN ABAJO */


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
            cout << "RESPUESTA RECIBIDA" << endl;
        //     // "reiniciamos" el buffer
        //     size = 0;
        //     for (int i = 0; respuesta != MENS_FIN && i < BUFFSIZE; i++) {
                
        //         buffer[i] = respuesta;
        //         size++;
        //         len = MESSAGE.length();
        //         n = len/500;
        //         for(int k = 0; k < 25; k++) {
        //             if(k <  n) send_bytes = chanGestor.Send(socket_fd_gestor, MESSAGE.substr(k*500,500));
        //             else if(k == n) send_bytes = chanGestor.Send(socket_fd_gestor, MESSAGE.substr(k*500,500)+"$$");
        //             else send_bytes = chanGestor.Send(socket_fd_gestor, " ");
        //             if(send_bytes == -1) {
        //                 cerr << "Error al enviar datos al gestor: " << strerror(errno) << endl;
        //                 // Cerramos el socket
        //                 chanGestor.Close(socket_fd_gestor);
        //                 exit(1);
        //             }
        //         }

        //         respuesta = "";
        //         for(int i = 0; i < 25; i++) {
        //             // Recibimos la respuesta del servidor gestor
        //             read_bytes = chanGestor.Recv(socket_fd_gestor, aux, LENGTH);
                        
        //             if(read_bytes == -1) {
        //                 cerr << "Error al recibir datos: " << strerror(errno) << endl;
        //                 // Cerramos el socket
        //                 chanGestor.Close(socket_fd_gestor);
        //                 exit(1);
        //             }
        //             respuesta.append(aux);
        //         }
        //         respuesta = respuesta.substr(0,respuesta.find("$$"));
        //     }
            if (respuesta == MENS_FIN) {
                finDatos = true;
            }

        }
        /********************************** GESTIÓN HILOS ***********************************/
        // th_globalAnalyzer.join();
        // // th_tagsAnalyzer.join();
        // th_tagsAnalyerMain.join();
        // th_tagsAnalyzerRT.join();
        // th_tagsAnalyzerO.join();
        // th_tagsAnalyzerM.join();
        /* despues de pruebas descomentar
        thread th_userAnalyzer[10];

        for (int i = 0; i < tamanyo(listaTags) && i < 10; i++) {
            th_userAnalyzer[i] = thread(&userAnalyzer,i);
        }

        for (int i = 0; i < tamanyo(listaTags); i++) {
            th_userAnalyzer[i].join();
        }
        
        thread th_cliente(&cliente);
        th_cliente.join();
        */
        /************************************************************************************/
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
