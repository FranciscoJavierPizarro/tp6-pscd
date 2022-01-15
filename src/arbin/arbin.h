//------------------------------------------------------------------------------
// File:   arbin.h
// Authors: Inés Román Gracia
// Axel Isaac Pazmiño Ortega
// Alicia Lázaro Huerta
// Francisco Javier Pizarro Martínez
// Date:   15/01/2022
//------------------------------------------------------------------------------


#ifndef ARBIN_H
#define ARBIN_H
#include "pila.h"
#include <iostream>
using namespace std;

// Interfaz del TAD genérico arbin

/* Es un TAD genérico para representar colecciones de ítems (ITF) en la que
no puede haber dos ITFs con el mismo identificador. A un ITF se le requiere tener definidas: una operación
identificador, que devuelve una cadena que identifica a ese ítem, una operación que devuelve un entero con
el valor del ítem, una operación que aumente el valor del ítem en 1 y una operación que compare dos valores 
de dos ítems */
template<typename ITF> struct arbin;

/* Crea una colección de ITF vacía, sin ítems */
template<typename ITF> void crear(arbin<ITF>& c);

/* Si en c no hay ningún ítem con identificador igual al de i, devuelve una colección igual a la
resultante de añadir el ítem i a la colección c en la posición que le corresponda siguiendo un orden 
lexicógrafico según su identificador. En caso contrario, se aumenta el valor del ítem en 1.*/
template<typename ITF> bool anyadir(const ITF& i, arbin<ITF>& c);

/* Devuelve verdad si y sólo si c no contiene ningún ítem.*/
template<typename ITF> bool esVacia(const arbin<ITF>& c);

/* Devuelve el número de ítems que hay en la colección c. */
template<typename ITF> int tamanyo(const arbin<ITF>& c);


// Las siguientes operaciones constituyen un iterador:

/* Inicializa el iterador para recorrer los ítems de la colección c, de forma que el siguiente ítem a visitar 
sea el que tiene el (lexicográficamente) menor identificador (situación de no haber visitado ningún ítem). */
template<typename ITF> void iniciarIterador(arbin<ITF>& c);

/* Devuelve verdad si queda algún ítem por visitar con el iterador de la colección c, devuelve
 falso si ya se ha visitado el último ítem. */
template<typename ITF> bool existeSiguiente(const arbin<ITF>& c);

/* Si quedan ítems por visitar, devuelve el siguiente ítem a visitar con el iterador de la colección c, prepara el 
iterador de la colección c para que se pueda visitar el siguiente ítem y la función devuelve true. En caso contrario, devuelve
la colección c y la función devuelve false. */
template<typename ITF> bool siguiente(arbin<ITF>& c, ITF& i);


// Las siguientes funciones son auxiliares de tipo interno:

/* Este procedimiento es auxiliar para el de anyadir */
template<typename ITF> bool anyadirRec(const ITF& i, typename arbin<ITF>::nodo*& aux);


// Declaración: 
template<typename ITF> struct arbin{
	friend void crear<ITF> (arbin<ITF>& c);
	friend bool anyadir<ITF> (const ITF& i, arbin<ITF>& c);
    friend bool esVacia<ITF> (const arbin<ITF>& c);
    friend int tamanyo<ITF> (const arbin<ITF>& c);
    friend void iniciarIterador<ITF> (arbin<ITF>& c);
    friend bool existeSiguiente<ITF> (const arbin<ITF>& c);
    friend bool siguiente<ITF> (arbin<ITF>& c, ITF& i);
	private: // Declaración interna del tipo:
		struct nodo{
            ITF dato;
            nodo *der;
            nodo *izq;
        };
        nodo *raiz;
        pila<nodo*> iter;
        int total;
        friend bool anyadirRec<ITF> (const ITF& i, typename arbin<ITF>::nodo*& aux);
        };

// Implementación: 

template<typename ITF> void crear(arbin<ITF>& c){
    c.total = 0;
    c.raiz = nullptr;
}

template<typename ITF> bool anyadir(const ITF& i, arbin<ITF>& c){
    bool anyadido = false;
    if(anyadirRec(i, c.raiz)){
        c.total++;
        anyadido = true;
    }
    return anyadido;
}

template<typename ITF> bool anyadirRec(const ITF& i, typename arbin<ITF>::nodo*& aux){
    bool anyadido;
    if(aux == nullptr){
        aux = new typename arbin<ITF>::nodo;
        aux->izq = nullptr;
        aux->der = nullptr;
        aux->dato = i;
        anyadido = true;
    }
    else{
        if(identificador(i) < identificador(aux->dato)){
            anyadido = anyadirRec(i, aux->izq);
        }
        else if(identificador(i) > identificador(aux->dato)){
            anyadido = anyadirRec(i, aux->der);
        }
        else{ // identificador(i) == identificador(aux->dato)
            aumentarValor(aux->dato);
        }
    }
    return anyadido;
}

template<typename ITF> bool esVacia(const arbin<ITF>& c){
    return c.total == 0;
}

template<typename ITF> int tamanyo(const arbin<ITF>& c){
    return c.total;
}

template<typename ITF> void iniciarIterador(arbin<ITF>& c){
    crear(c.iter);
    typename arbin<ITF>::nodo *aux = c.raiz;
    while(aux != nullptr){
        apilar(aux, c.iter);
        aux = aux->izq;
    }
}

template<typename ITF> bool existeSiguiente(const arbin<ITF>& c){
    return !esVacia(c.iter);
}

template<typename ITF> bool siguiente(arbin<ITF>& c, ITF& i){
    bool ok;
    if(existeSiguiente(c)){
        ok = true;
        typename arbin<ITF>::nodo *aux, *aux2;
        cima(aux, c.iter);
        desapilar(c.iter);
        i = aux->dato;
        aux = aux->der;
        while(aux != nullptr){
            aux2 = aux;
            apilar(aux2, c.iter);
            aux = aux->izq;
        }
    }
    else{
        ok = false;
    }
    return ok;
}

#endif