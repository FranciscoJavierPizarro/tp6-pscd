//---------------------------------------------------------------------------------------------
// Archivo: nodoTags.cpp
// Autores: Axel Isaac Pazmiño Ortega (817627) y Alicia Lázaro huerta (820574)
// Fecha: 11/01/2022
//---------------------------------------------------------------------------------------------
#include "nodoTag.h"

// Implementacion de las operaciones del TAD nodoTags

void iniciarNodoTag(nodoRT& nodo, const string ident) {
    nodo.Hashtag = ident;
    nodo.apHashtag = 1;
    nodo.quienAH = string();
    nodo.apHashtagT = 0;
    nodo.quienAHT = string();
    nodo.apHashtagRT = 0;
    nodo.quienAHRT = string();
    nodo.apHashtagM = 0;
    nodo.quienAHM = string();
}

/* FUNCIONES PARA MOSTRAR PARTES DEL NODO */

string identificador(const nodoRT nodo) {
    return nodo.Hashtag;
}

int total(const nodoRT nodo) {
    return nodo.apHashtag;
}

string mostrarQuienTotal(const nodoRT nodo) {
    return nodo.quienAH;
}

int mostrarHashtagT(const nodoRT nodo) {
    return nodo.apHashtagT;
}

string mostrarQuienHT(const nodoRT nodo) {
    return nodo.quienAHT;
}

int mostrarHashtagRT(const nodoRT nodo) {
    return nodo.apHashtagRT;
}

string mostrarQuienHRT(const nodoRT nodo) {
    return nodo.quienAHRT;
}

int mostrarHashtagM(const nodoRT nodo) {
    return nodo.apHashtagM;
}

string mostrarQuienHM(const nodoRT nodo) {
    return nodo.quienAHM;
}

/* FUNCIONES PARA MODIFICAR PARTES DEL NODO */

void actTotal(nodoRT& nodo, const int ap) {
    nodo.apHashtag = nodo.apHashtag + ap;
}

void actQuienTotal(nodoRT& nodo,const string autor) {
    if (nodo.quienAH.empty()) {
        nodo.quienAH = autor;
    }
    else {
        nodo.quienAH = nodo.quienAH + "/" + autor;
    }
    
}

void actApHashtagT(nodoRT& nodo, const int ap) {
    nodo.apHashtagT = nodo.apHashtagT + ap;
}

void actQuienHashtagT(nodoRT& nodo, const string autor) {
    if (nodo.quienAHT.empty()) {
        nodo.quienAHT = autor;
    }
    else {
        nodo.quienAHT = nodo.quienAHT + "/" + autor;
    }
}

void actApHashtagRT(nodoRT& nodo, const int ap) {
    nodo.apHashtagRT = nodo.apHashtagRT + ap;
}

void actQuienHashtagRT(nodoRT& nodo, const string autor) {
    if (nodo.quienAHRT.empty()) {
        nodo.quienAHRT = autor;
    }
    else {
        nodo.quienAHRT = nodo.quienAHRT + "/" + autor;
    }
}

void actApHashtagM(nodoRT& nodo, const int ap) {
    nodo.apHashtagM = nodo.apHashtagM + ap;
}

void actQuienHashtagM(nodoRT& nodo,const string buffer) {
    if (nodo.quienAHM.empty()) {
        nodo.quienAHM = buffer;
    }
    else {
        nodo.quienAHM = nodo.quienAHM + "/" + buffer;
    }
}