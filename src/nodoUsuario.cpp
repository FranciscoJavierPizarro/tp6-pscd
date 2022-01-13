//---------------------------------------------------------------------------------------------
// Archivo: nodoUsuario.cpp
// Autores: Axel Isaac Pazmiño Ortega (817627) y Alicia Lázaro huerta (820574)
// Fecha: 11/01/2022
//---------------------------------------------------------------------------------------------
#include "nodoUsuario.h"

void iniciarNodoUser(nodoUser& nodo,const string ident) {
    nodo.autor = ident;
    nodo.actividad = 1;
    nodo.usoTweet = 0;
    nodo.usoTMencion = 0;
    nodo.usoRT = 0;
}

/* FUNCIONES PARA MOSTRAR PARTES DEL NODO */

string identificador(const nodoUser nodo) {
    return nodo.autor;
}

int total(const nodoUser nodo) {
    return nodo.actividad;
}

int mostrarUsoTweet(const nodoUser nodo) {
    return nodo.usoTweet;
}

int mostrarUsoTMencion(const nodoUser nodo) {
    return nodo.usoTMencion;
}

int mostrarUsoRT(const nodoUser nodo) {
    return nodo.usoRT;
}

/* FUNCIONES PARA MODIFICAR PARTES DEL NODO */

void actTotal(nodoUser& nodo, const int ap) {
    nodo.actividad = nodo.actividad + ap;
}

void actUsoTweet(nodoUser& nodo, const int ap) {
    nodo.usoTweet = nodo.usoTweet + ap;
}

void actUsoTMencion(nodoUser& nodo, const int ap) {
    nodo.usoTMencion = nodo.usoTMencion + ap;
}

void actUsoRT(nodoUser& nodo, const int ap) {
    nodo.usoRT = nodo.usoRT + ap;
}