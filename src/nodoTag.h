//-------------------------------------------------------------------------------------------
// Archivo:   nodoTag.h
// Autor:     Axel Isaac Pazmiño Ortega, NIP:817627, y Alicia Lázaro Huerta, NIP: 820574
// Fecha:     8/1/2022
//-------------------------------------------------------------------------------------------
#ifndef _NODO_TAG_H_
#define _NODO_TAG_H_
#include <iostream>
using namespace std;

/* Pre-declaraciones: */
struct nodoRT;
/* nodoRT es un nodo que coniene el Hashtag que es,
 * el número de apariciones totales del hashtag, quienes han puesto cada aparición del hashtag,
 * tweets originales con el tag, quienes los han escrito
 * retweets con el tag, quienes los han escrito, tweets con menciones y con el tag,
 * y quien ha escrito el tweet con menciones y a quienes ha mencionado.
 */

void iniciarNodoTag(nodoRT& nodo, const string ident);
/* Devuelve un nodo inicializado con identificador ident
 * y 1 en apHashtag
 */

/* FUNCIONES PARA MOSTRAR PARTES DEL NODO */

string identificador(const nodoRT nodo);
/* Devuelve el hashtag que identifica al nodo. */

int total(const nodoRT nodo);
/* Devuelve el número de apariciones totales del hashtag */

string mostrarQuienTotal(const nodoRT nodo);
/* Devuelve una cadena que está formada por: cada aparición del hashtag tiene
 * asociado un autor escrito en la cadena separado con '/'
 * sea que el hashtag estaba en un retweet, en un tweet, apareciera 20 veces en un tweet, etc...
 */

int mostrarHashtagT(const nodoRT nodo);
/* Devuelve el número de tweets con el hashtag */

string mostrarQuienHT(const nodoRT nodo);
/* Devuelve una cadena formada por: cada tweet con el hashtag, tiene asociado un autor
 * escrito en la cadena separado con '/'
 */

int mostrarHashtagRT(const nodoRT nodo);
/* Devuelve el número de retweets con el hashtag */

string mostrarQuienHRT(const nodoRT nodo);
/* Devuelve una cadena formada por: cada retweet con el hashtag, tiene asociado un autor
 * escrito en la cadena separado con '/'
 */

int mostrarHashtagM(const nodoRT nodo);
/* Devuelve número de tweets con el Hashtag y contienen menciones */

string mostrarQuienHM(const nodoRT nodo);
/* Devuelve una cadena formada por: cada tweet con el hashtag, tiene asociado un autor
 * seguido con el separador '&' y las menciones que escribe que empiezan por '@'.
 * Cada tweet está separado por '/'.
 */

/* FUNCIONES PARA MODIFICAR PARTES DEL NODO */

void actTotal(nodoRT& nodo, const int ap);
/* Actualiza el valor de apHashtag sumandolo con ap */

void actQuienTotal(nodoRT& nodo,const string autor);
/* Actualiza el valor de quienAH concatenandolo con con el separador '/'
 * seguido con autor.
 */

void actApHashtagT(nodoRT& nodo, const int ap);
/* Actualiza el valor de apHashtagT sumandolo con ap */

void actQuienHashtagT(nodoRT& nodo, const string autor);
/* Actualiza el valor de quienAHT concatenandolo con el separador '/'
 * seguido con autor.
 */

void actApHashtagRT(nodoRT& nodo, const int ap);
/* Actualiza el valor de apHashtagRT sumandolo con ap */

void actQuienHashtagRT(nodoRT& nodo, const string autor);
/* Actualiza el valor de quienAHRT concatenandolo con el separador '/'
 * seguido con el autor.
 */

void actApHashtagM(nodoRT& nodo, const int ap);
/* Actualiza el valor de apHashtagM sumandolo con ap */

void actQuienHashtagM(nodoRT& nodo,const string buffer);
/* Actualiza el valor de quienAHM concatenandolo con el separador '/',
 * y el buffer que tiene la estructura:
 * autor seguido del separador '&' seguido de las menciones que empieza cada mención con '@'.
 */

struct nodoRT {
    friend void iniciarNodoTag(nodoRT& nodo, const string ident);
    /* FUNCIONES PARA MOSTRAR PARTES DEL NODO */
    friend string identificador(const nodoRT nodo);
    friend int total(const nodoRT nodo);
    friend string mostrarQuienTotal(const nodoRT nodo);
    friend int mostrarHashtagT(const nodoRT nodo);
    friend string mostrarQuienHT(const nodoRT nodo);
    friend int mostrarHashtagRT(const nodoRT nodo);
    friend string mostrarQuienHRT(const nodoRT nodo);
    friend int mostrarHashtagM(const nodoRT nodo);
    friend string mostrarQuienHM(const nodoRT nodo);
    /* FUNCIONES PARA MODIFICAR PARTES DEL NODO */
    friend void actTotal(nodoRT& nodo, const int ap);
    friend void actQuienTotal(nodoRT& nodo,const string autor);
    friend void actApHashtagT(nodoRT& nodo, const int ap);
    friend void actQuienHashtagT(nodoRT& nodo, const string autor);
    friend void actApHashtagRT(nodoRT& nodo, const int ap);
    friend void actQuienHashtagRT(nodoRT& nodo, const string autor);
    friend void actApHashtagM(nodoRT& nodo, const int ap);
    friend void actQuienHashtagM(nodoRT& nodo, const string buffer);
    private:
        string Hashtag;
        int apHashtag;
        string quienAH;
        int apHashtagT;
        string quienAHT;
        int apHashtagRT;
        string quienAHRT;
        int apHashtagM;
        string quienAHM;
};
#endif