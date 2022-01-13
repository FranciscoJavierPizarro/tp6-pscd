//-------------------------------------------------------------------------------------------
// Archivo:   listaGenerica.h
// Autor:     Axel Isaac Pazmiño Ortega, NIP:817627, y Alicia Lázaro Huerta, NIP: 820574
// Fecha:     8/1/2022
//-------------------------------------------------------------------------------------------
#ifndef _LISTA_GENERICA_H_
#define _LISTA_GENERICA_H_
#include <iostream>
#include <string>
using namespace std;

// Interfaz del TAD. Pre-declaraciones:
template<typename N> struct lista;
/* El TAD generico lista
 * representa una lista enlazada de nodos E ordenada parcialmente:
 * los 10 primeros nodos de la lista son los nodos de mayor valor 
 * (de mayor a menor) de toda la lista. 
 * El valor y el identificador por el que se mide un nodo
 * es según las funciones exigidas a E:
 * identificador(const N nodo) (devuelve la clave del nodo ) y 
 * total(const N nodo) (devuelve valor del elemento).
 */

template<typename N> void crear(lista<N>& l);
/* Crea una lista vacía, sin nodos. */

template<typename N> void vaciarLista(lista<N>& l);
/* vacía la lista l eliminando los nodos.
 * En caso de ser vacía se queda igual
 */

template<typename N> int obtenerNodo(const lista<N> l, const string ident, N*& direccion);
/* si hay un nodo de la lista l con el mismo identificador que ident se devuelve su indice
 * y se devuelve un puntero de N en direccion.
 * Devuelve -1 en caso contrario
 * Parcial: no está definido a que apunta direccion en caso de no encontrarse
 */

template<typename N> void actualizarNodo(lista<N>& l,const int ind);
/* Revisa el total del N de indice ind y si hace falta lo reordena.
 * Parcial: no está definido el funcinonamiento si ind
 * no es menor que el total de N en la lista
 */

template<typename N> void insertarNodo(lista<N>& l, const N nodo);
/* Añade el nodo a la lista l en la posición según su valor.
 * Parcial: no está definido el funcionamiento si ya hay un nodo con el mismo identificador
 */

template<typename N> int tamanyo(const lista<N> l);
/* Devuelve el número de nodos que tiene la lista l*/

/* operaciones del iterador */

template<typename N> void iniciarIterador(lista<N>& lista);
/* Iniccializa el iterador */

template<typename N> bool existeSiguiente(const lista<N> lista);
/* Devuelve verdadero si existe otro N más que dar.
 * Devuelve falso en caso contrario .
 */
template<typename N> N siguiente(lista<N>& lista);
/* Devuelve N y deja preparado para mostrar el siguiete 
 * Parcial: la funcion no está definida si no hay un N que devolver
 */

/* Declaracion */

template<typename N> struct lista {
    friend void crear<N>(lista<N>& l);
    friend void vaciarLista<N>(lista<N>& l);
    friend int obtenerNodo<N>(const lista<N> l, const string ident, N*& direccion);
    friend void insertarNodo<N>(lista<N>& l, const N nodo);
    friend void actualizarNodo<N>(lista<N>& l,const int ind);
    friend int tamanyo<N>(const lista<N> l);
    /* funciones del iterador */
    friend void iniciarIterador<N>(lista<N>& lista);
    friend bool existeSiguiente<N>(const lista<N> lista);
    friend N siguiente<N>(lista<N>& lista);
private:
	/* campos del TAD*/
    struct celda {
        N nodo;
        celda* next;
        celda* last;
    };
	celda* base;
    celda* cima;
    celda* iter;
	int total;
};

/* implementación */

template<typename N>
void crear(lista<N>& l) {
    l.base = nullptr;
    l.cima = nullptr;
    l.total = 0;
}

template<typename N> 
void vaciarLista(lista<N>& l) {
    if (l.total > 0) {
        delete &l.base[l.total-1];
        l.total--;
        vaciarLista(l);
    }
    else {
        l.cima = nullptr;
        l.base = nullptr;
    }
}

template<typename N> 
int obtenerNodo(const lista<N> l, const string ident, N*& direccion) {
    typename lista<N>::celda* aux = l.base;
    int indEncontrado = -1;
    int indice = 0;
    while (aux != nullptr && indEncontrado < 0) {
        if (ident == identificador(aux->nodo)) {
            direccion = &aux->nodo;
            indEncontrado = indice;
        }
        aux = aux->next;
        indice++;
    }
    return indEncontrado;
}

template<typename N> 
void actualizarNodo(lista<N>& l,const int ind) {
    typename lista<N>::celda* nodoPre;
    typename lista<N>::celda* nodoPost;
    typename lista<N>:: celda* nodoNew;
    nodoNew = &l.base[ind];
    if (ind > 9 && total(l.base[ind].nodo) > total(l.base[9].nodo)) {
        bool stop = false;
        for (int i = 9; i >= 0 && !stop; i--) {
            if (total(l.base[ind].nodo) <= total(l.base[i].nodo)) {
                stop = true;
                // quitamos el nodo de su antiguo lugar
                nodoPre = &l.base[ind-1];
                nodoPost = &l.base[ind+1];
                nodoPre->next = nodoPost;
                nodoPost->last = nodoPre;
                // lo colocamos en su nueva posición
                nodoPre = &l.base[i];
                nodoPost = &l.base[i+1];
                nodoNew->last = nodoPre;
                nodoNew->next = nodoPost;
                nodoPre->next = nodoNew;
                nodoPost->last = nodoNew;
            }
        }
        if (!stop) { // lo colocamos al principio
            nodoPre = l.base;
            l.base = nodoNew;
            nodoNew->last = nullptr;
            nodoNew->next = nodoPre;
        }
    }
    else {
        bool stop = false;
        for (int i = ind - 1; i >= 0 && !stop; i--) {
            if (total(l.base[ind].nodo) <= total(l.base[i].nodo)) {
                stop = true;
                // quitamos el nodo de su antiguo lugar
                nodoPre = &l.base[ind-1];
                nodoPost = &l.base[ind+1];
                nodoPre->next = nodoPost;
                nodoPost->last = nodoPre;
                // lo colocamos en su nueva posición
                nodoPre = &l.base[i];
                nodoPost = &l.base[i+1];
                nodoNew->last = nodoPre;
                nodoNew->next = nodoPost;
                nodoPre->next = nodoNew;
                nodoPost->last = nodoNew;
            }
        }
    }
}

template<typename N> 
void insertarNodo(lista<N>& l, const N nodo) {
    typename lista<N>::celda* nodoPre;
    typename lista<N>::celda* nodoPost;
    typename lista<N>:: celda* nodoNew;
    if (l.total >= 10) { // significa que hay igual o más de 10 elementos
        if (total(nodo) > total(l.base[9].nodo)) {
            bool stop = false;
            for (int i = 9; i >= 0 && !stop; i--) {
                if (total(nodo) <= total(l.base[i].nodo)) {
                    stop = true;
                    // inserción detrás de lista.base[i];
                    nodoPre = &l.base[i];
                    nodoPost = &l.base[i+1];
                    nodoNew = new typename lista<N>::celda;
                    nodoPre->next = nodoNew;
                    nodoNew->nodo = nodo;
                    nodoNew->next = nodoPost;
                    nodoNew->last = nodoPre;
                    l.total++;
                }
            }
            if (!stop) { // inserción al principio
                nodoPre = l.base;
                nodoNew = new typename lista<N>::celda;
                nodoNew->nodo = nodo;
                l.base = nodoNew;
                nodoNew->last = nullptr;
                nodoNew->next = nodoPre;
                l.total++;
            }
        }
        else {  // inserción al final
            l.cima = new typename lista<N>::celda;
            l.cima->last = &l.base[l.total-1];
            l.cima->nodo = nodo;
            l.cima->next = nullptr;
            l.total++;
        }
    }
    else { // hay menos de 10
        bool stop = false;
        for (int i = l.total-1; i >= 0 && !stop; i--) {
            if (total(nodo) <= total(l.base[i].nodo)) {
                stop = true;
                // inserción detrás de lista.base[i];
                nodoPre = &l.base[i];
                nodoPost = &l.base[i+1];
                nodoNew = new typename lista<N>::celda;
                nodoPre->next = nodoNew;
                nodoNew->nodo = nodo;
                nodoNew->next = nodoPost;
                nodoNew->last = nodoPre;
                // actualizar total
                l.total++;
            }
        }
        if (!stop) { // inserción al principio
            nodoPre = l.base;
            nodoNew = new typename lista<N>::celda;
            nodoNew->nodo = nodo;
            l.base = nodoNew;
            nodoNew->last = nullptr;
            nodoNew->next = nodoPre;
            l.total++;
        }
    }
}

template<typename N> 
int tamanyo(const lista<N> l) {
    return l.total;
}

template<typename N> 
void iniciarIterador(lista<N>& lista) {
    lista.iter = lista.base;
}

template<typename N> 
bool existeSiguiente(const lista<N> lista) {
    bool haySiguiente;
    if (lista.iter == nullptr) {
        haySiguiente = false;
    }
    else {
        haySiguiente = true;
    }
    return haySiguiente;
}

template<typename N> 
N siguiente(lista<N>& lista) {
    typename lista<N>::celda* aux = lista.iter;
    lista.iter = lista.iter->next;
    return aux->nodo;
}

#endif