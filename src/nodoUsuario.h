//-------------------------------------------------------------------------------------------
// Archivo:   nodoUsuario.h
// Autor:     Axel Isaac Pazmiño Ortega, NIP:817627, y Alicia Lázaro Huerta, NIP: 820574
// Fecha:     8/1/2022
//-------------------------------------------------------------------------------------------
#ifndef _NODO_USUARIO_H_
#define _NODO_USUARIO_H_
#include <iostream>
using namespace std;

/* Pre-declaraciones: */
struct nodoUser;
/* nodoUser es un nodo en el que hay un usuario/autor, número de usos totales del usuario,
 * cuantas veces escribe tweets, cuantos tweets hay con menciones del mismo
 * y cuantas veces hace retweet. 
 */

void iniciarNodoUser(nodoUser& nodo,const string ident);
/* Devuelve un nodo inicializado con identificador ident
 * y 1 en actividad
 */

/* FUNCIONES PARA MOSTRAR PARTES DEL NODO */

string identificador(const nodoUser nodo);
/* Devuelve el autor que identifica al nodo. */

int total(const nodoUser nodo);
/* Devuelve el número de usos totales del autor*/

int mostrarUsoTweet(const nodoUser nodo);
/* Devuelve número de tweets del autor */

int mostrarUsoTMencion(const nodoUser nodo);
/* Devuelve número de menciones al autor */

int mostrarUsoRT(const nodoUser nodo);
/* Devuelve número de retweets */

/* FUNCIONES PARA MODIFICAR PARTES DEL NODO */

void actTotal(nodoUser& nodo, const int ap);
/* Actualiza el valor de actividad sumandolo con ap */

void actUsoTweet(nodoUser& nodo, const int ap);
/* Actualiza el valor de usoTweet sumandolo con ap */

void actUsoTMencion(nodoUser& nodo, const int ap);
/* Actualiza el valor de usoTMencion sumandolo con ap */

void actUsoRT(nodoUser& nodo, const int ap);
/* Actualiza el valor de usoRT sumandolo con ap */

struct nodoUser {
    friend void iniciarNodoUser(nodoUser& nodo, const string ident);
    /* FUNCIONES PARA MOSTRAR PARTES DEL NODO */
    friend string identificador(const nodoUser nodo);
    friend int total(const nodoUser nodo);
    friend int mostrarUsoTweet(const nodoUser nodo);
    friend int mostrarUsoTMencion(const nodoUser nodo);
    friend int mostrarUsoRT(const nodoUser nodo);
    /* FUNCIONES PARA MODIFICAR PARTES DEL NODO */
    friend void actTotal(nodoUser& nodo, const int ap);
    friend void actUsoTweet(nodoUser& nodo, const int ap);
    friend void actUsoTMencion(nodoUser& nodo, const int ap);
    friend void actUsoRT(nodoUser& nodo, const int ap);
    private:
        string autor;
        int actividad;
        int usoTweet;
        int usoTMencion;
        int usoRT;
};

#endif