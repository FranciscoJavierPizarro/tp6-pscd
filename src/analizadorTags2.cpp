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
using namespace std;

// VARIABLES GLOBALES
const int BUFFSIZE = 500;
string buffer[BUFFSIZE];
int size = 0;

// TAD lista enlazada 
/* en cada nodo, está el Hashtag que es, el número de apariciones totales, en tweets originales,
 * en retweets y en tweets con menciones.
 * La lista está parcialmente ordenada los 10 primeros nodos de mayor a menor según
 * el número total de apariciones de su hashtag.
 */
struct resultadosTags {
    struct nodoRT {
        string Hashtag = string();
        int apHashtag = 0;
        string quienAH = string();
        int apHashtagT = 0;
        string quienAHT = string();
        int apHashtagRT = 0;
        string quienAHRT = string();
        int apHashtagM = 0;
        string quienAHM = string();
        nodoRT* next = nullptr;
    };
    nodoRT* init = nullptr;
    int totalHashtags = 0;
};

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

// TAD lista enlazada
/* Dado un Hashtag: en cada nodo, hay un usuario, número de usos totales del usuario,
 * cuantas veces escribe tweets, cuantas veces escribe tweets con menciones y cuantas veces hace retweet. 
 * La lista está parcialmente ordenada los 10 primeros nodos de mayor a menor según
 * el número total de uso deL hashtag por el usuario.
 */
struct resultadosHashtag {
    string Hashtag;
    struct nodoUser {
        string autor;
        int actividad;
        int usoTweet;
        int usoTMencion;
        int usoRT;
        nodoUser* next;
    };
    nodoUser* next = nullptr;
    int totalUsers = 0;
};

resultadosHashtag top10[10];

const resultadosTags* listaTags;

// GESTIÓN SINCRONIZACIÓN:
Semaphore heTerminado(0);
Semaphore sigoTags(0);
Semaphore sigoGlobales(0);
bool finDatos = false;

/********************************** FUNCIONES AUXILIARES ***********************************/
// Pre: soc tiene establecida una conexión con el gestor de colas y debe haber suficientes datos como para llenar el buffer
// Post: devuelve el buffer inicializado con los primeros datos a procesar
void inicializarBuffer(Socket& soc,int socket_fd_gestor, const string mensaje, const int leng) {
    string respuesta;
    int read_bytes,send_bytes;
    send_bytes = soc.Send(socket_fd_gestor, mensaje);
            
    if(send_bytes == -1) {
        cerr << "Error al enviar datos: " << strerror(errno) << endl;
        // Cerramos el socket
        soc.Close(socket_fd_gestor);
        exit(1);
    }

    for (int i = 0; i < BUFFSIZE; i++) {
        read_bytes = soc.Recv(socket_fd_gestor, respuesta, leng);
        
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            soc.Close(socket_fd_gestor);
            exit(1);
        } 
        buffer[i] = respuesta;

        send_bytes = soc.Send(socket_fd_gestor, mensaje);
            
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            soc.Close(socket_fd_gestor);
            exit(1);
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

// Pre: hashtagOrig y hashtagComp son hashtags válidos dentro de las normas de twitter.
// Post: compara sin son el mismo hashtag, no solo comparando carácter a caracter, sino también según
//       las igualdades entre hashtags según la política de twitter:
//       no hay diferencias entre mayúscula y minúscula
//       tampoco entre llevar tilde o no
//       ni entre llevar ñ o n
bool estandarizarHashtag(const string hashtagOrig,string& hashtagComp) {

}

// Pre:
// Post:
void ordenarTags(const resultadosTags* listaTags, const int numTags,const resultadosTags::nodoRT* punteroTag,const int posTag,const bool estaEnLaLista) {

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
    // ordenar
}

// Pre: ---
// Post: mientras no hay más datos que leer,lee del buffer, procesa cada elemento hasta size, y el resultado
//       (descrito en la declaración de su lista) lo la almacena en listaTags. 
void tagsAnalyzer() {
    // variables
    bool fin = false;
    while (!fin) {
        resultadosTags::nodoRT* listaAux = nullptr;
        for (int i = 0; i < size; i++) {
            string tokenTags = obtenerTokenLista(5,buffer[i]);
            int tagPointer = 0;
            const char delim = '%';
            const char delimAutor = '&';
            const char delimHashtag = '#';
            bool inRange;
            string tagsAux,tweetAux,hashtag,autor;
            obtenerToken(tagPointer,tokenTags,delim,tagsAux,inRange); // deja solo tags en tagsAux
            if (inRange) { // descartamos procesar el mensaje si es vacío
                // PROCESAMIENTO DE TAGS
                int hashtagPointer = 1; // ignoramos el primer "#"
                obtenerToken(hashtagPointer,tweetAux,delimAutor,hashtag,inRange); // obtenemos el hashtag
                obtenerToken(hashtagPointer,tweetAux,delimHashtag,autor,inRange); // obtenemos el autor
                while (inRange) {
                    resultadosTags::nodoRT* aux = listaAux;
                    bool encontrado = false;
                    while (aux != nullptr && !encontrado) {
                        if (estandarizarHashtag(aux->Hashtag,hashtag)) {
                            aux->apHashtag++;
                            aux->quienAH = aux->quienAH + autor + "/";
                            encontrado = true;
                        }
                        else {
                            aux = aux->next;
                        }
                    }
                    if (!encontrado) { //insertamos el nuevo hashtag
                        aux = listaAux;
                        listaAux = new resultadosTags::nodoRT;
                        listaAux->Hashtag = hashtag;
                        listaAux->apHashtag = 1;
                        listaAux->quienAH = autor + "/";
                        listaAux->next = aux;
                    }
                    obtenerToken(hashtagPointer,tweetAux,delimAutor,hashtag,inRange);
                    obtenerToken(hashtagPointer,tweetAux,delimHashtag,autor,inRange); 
                }
            }
            obtenerToken(tagPointer,tokenTags,delim,tagsAux,inRange); // deja solo tagsRT en tagsAux
            if (inRange) {
                int rtPointer = 0;
                int hashtagPointer = 1;// ignoramos el primer "#"
                // PROCESAMIENTO DE TAGS_RT
                obtenerToken(rtPointer,tagsAux,delim,tweetAux,inRange); // deja solo rt en tweetAux
                while (inRange) {
                    obtenerToken(hashtagPointer,tweetAux,delimAutor,hashtag,inRange); // obtenemos el hashtag
                    obtenerToken(hashtagPointer,tweetAux,delimHashtag,autor,inRange); // obtenemos el autor
                    while (inRange) {
                        bool encontrado = false;
                        while (!encontrado) {
                            if (estandarizarHashtag(listaAux->Hashtag,hashtag)) {
                                listaAux->apHashtagRT++;
                                listaAux->quienAHRT = listaAux->quienAHRT + autor + "/";
                                encontrado = true;
                            }
                            else {
                                listaAux = listaAux->next;
                            }
                        }
                        obtenerToken(hashtagPointer,tweetAux,delimAutor,hashtag,inRange);
                        obtenerToken(hashtagPointer,tweetAux,delimHashtag,autor,inRange); 
                    }
                    obtenerToken(rtPointer,tagsAux,delim,tweetAux,inRange); // deja solo rt en tweetAux
                }
            }
            obtenerToken(tagPointer,tokenTags,delim,tagsAux,inRange); // deja solo tagsO en tagsAux
            if (inRange) {
                int tweetPointer = 0;
                int hashtagPointer = 1;// ignoramos el primer "#"
                // PROCESAMIENTO DE TAGS_O
                obtenerToken(tweetPointer,tagsAux,delim,tweetAux,inRange); // deja solo tweet en tweetAux
                while (inRange) {
                    obtenerToken(hashtagPointer,tweetAux,delimAutor,hashtag,inRange); // obtenemos el hashtag
                    obtenerToken(hashtagPointer,tweetAux,delimHashtag,autor,inRange); // obtenemos el autor
                    while (inRange) {
                        bool encontrado = false;
                        while (!encontrado) {
                            if (estandarizarHashtag(listaAux->Hashtag,hashtag)) {
                                listaAux->apHashtagT++;
                                listaAux->quienAHT = listaAux->quienAHT + autor + "/";
                                encontrado = true;
                            }
                            else {
                                listaAux = listaAux->next;
                            }
                        }
                        obtenerToken(hashtagPointer,tweetAux,delimAutor,hashtag,inRange);
                        obtenerToken(hashtagPointer,tweetAux,delimHashtag,autor,inRange); 
                    }
                    obtenerToken(tweetPointer,tagsAux,delim,tweetAux,inRange); // deja solo tweet en tweetAux
                }
            }
            obtenerToken(tagPointer,tokenTags,delim,tagsAux,inRange); // deja solo tagsM en tagsAux
            if (inRange) {
                int tweetMPointer = 0;
                int hashtagPointer = 1; // ignoramos el primer "#"
                int hP = 0;
                // PROCESAMIENTO DE TAGS_M
                obtenerToken(tweetMPointer,tagsAux,delim,tweetAux,inRange); // deja solo tweetM en tweetAux
                while (inRange) {
                    string menciones,hashtags;
                    obtenerToken(hashtagPointer,tweetAux,delimHashtag,menciones,inRange); // obtenemos las menciones
                    obtenerToken(hashtagPointer,tweetAux,delimAutor,hashtags,inRange); // obtenemos los hashtags
                    obtenerToken(hashtagPointer,tweetAux,delimHashtag,autor,inRange); // obtenemos el autor(delim se pone por defecto)
                    obtenerToken(hP,hashtags,delimHashtag,hashtag,inRange);
                    while (inRange) {
                        bool encontrado = false;
                        while (!encontrado) {
                            if (estandarizarHashtag(listaAux->Hashtag,hashtag)) {
                                listaAux->apHashtagM++;
                                listaAux->quienAHM = listaAux->quienAHM + autor + "&" + menciones + "/";
                                encontrado = true;
                            }
                            else {
                                listaAux = listaAux->next;
                            }
                        }
                        obtenerToken(hP,hashtags,delimHashtag,hashtag,inRange);
                    }
                    obtenerToken(tweetMPointer,tagsAux,delim,tweetAux,inRange); // deja solo tweetM en tweetAux
                }
            }
        }
        //INSERCIÓN EN LA LISTA DEFINITIVA:
        bool encontrado = false;
        resultadosTags::nodoRT* auxListaMain;
        resultadosTags::nodoRT* auxLista = listaAux;
        bool estaEnLaLista;
        while (auxLista != nullptr) {
            auxListaMain = listaTags->init;
            int i = 0;
            while ( auxListaMain != nullptr && !encontrado) {
                if (auxLista->Hashtag == auxListaMain->Hashtag) {
                    encontrado = true;
                    auxListaMain->apHashtag+=auxLista->apHashtag;
                    auxListaMain->apHashtagM+=auxLista->apHashtagM;
                    auxListaMain->apHashtagRT+=auxLista->apHashtagRT;
                    auxListaMain->apHashtagT+=auxLista->apHashtagT;
                    auxListaMain->quienAH+=auxLista->quienAH;
                    auxListaMain->quienAHM+=auxLista->quienAHM;
                    auxListaMain->quienAHRT+=auxLista->quienAHRT;
                    auxListaMain->quienAHT+=auxLista->quienAHT;

                    estaEnLaLista = true;
                    ordenarTags(listaTags,listaTags->totalHashtags,auxListaMain,i,estaEnLaLista);
                    listaAux = auxLista->next;
                    delete auxLista;
                }
                else {
                    auxListaMain = auxListaMain->next;
                    i++;
                }
            }
            if (!encontrado) { //insertamos el nuevo hashtag
                estaEnLaLista = false;
                i = -1; // no tiene posición
                ordenarTags(listaTags,listaTags->totalHashtags,auxLista,i,estaEnLaLista);
            }
            auxLista = auxLista->next;
        }

        heTerminado.signal();
        if (finDatos) {
            fin = true;
        }
        sigoTags.wait();
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
// Se encarga de gestionar la conexión con el gestor de colas y la gestión de los hilos
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
        const string MESSAGE = "READ_TAGS";
        string respuesta;
        int send_bytes,read_bytes;
        int LENGTH = 10240; //10 KB
        inicializarBuffer(chanGestor,socket_fd_gestor,MESSAGE,LENGTH);
        /********************************** GESTIÓN HILOS ***********************************/
        thread th_globalAnalyzer(&globalAnalyzer);
        thread th_tagsAnalyzer(&tagsAnalyzer);
        /************************************************************************************/

        while (!finDatos) {

            heTerminado.wait(2);

            send_bytes = chanGestor.Send(socket_fd_gestor, MESSAGE);
            
            if(send_bytes == -1) {
                cerr << "Error al enviar datos: " << strerror(errno) << endl;
                // Cerramos el socket
                chanGestor.Close(socket_fd_gestor);
                exit(1);
            }

            read_bytes = chanGestor.Recv(socket_fd_gestor, respuesta, LENGTH);
        
            if(read_bytes == -1) {
                cerr << "Error al recibir datos: " << strerror(errno) << endl;
                // Cerramos el socket
                chanGestor.Close(socket_fd_gestor);
                exit(1);
            } 
            
            // "reiniciamos" el buffer
            size = 0;

            for (int i = 0; respuesta != MENS_FIN && i < BUFFSIZE; i++) {
                
                buffer[i] = respuesta;
                size++;
                send_bytes = chanGestor.Send(socket_fd_gestor, MESSAGE);
            
                if(send_bytes == -1) {
                    cerr << "Error al enviar datos: " << strerror(errno) << endl;
                    // Cerramos el socket
                    chanGestor.Close(socket_fd_gestor);
                    exit(1);
                }

                read_bytes = chanGestor.Recv(socket_fd_gestor, respuesta, LENGTH);
            
                if(read_bytes == -1) {
                    cerr << "Error al recibir datos: " << strerror(errno) << endl;
                    // Cerramos el socket
                    chanGestor.Close(socket_fd_gestor);
                    exit(1);
                }
            }
            
            if (respuesta == MENS_FIN) {
                finDatos = true;
            }
        }

        /********************************** GESTIÓN HILOS ***********************************/
        th_globalAnalyzer.join();
        th_globalAnalyzer.join();

        thread th_userAnalyzer[10];

        for (int i = 0; i < listaTags->totalHashtags && i < 10; i++) {
            th_userAnalyzer[i] = thread(&userAnalyzer,i);
        }

        for (int i = 0; i < listaTags->totalHashtags; i++) {
            th_userAnalyzer[i].join();
        }

        thread th_cliente(&cliente);
        th_cliente.join();
        /************************************************************************************/

        cout << "BYE BYE" << endl;
        return 0;
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