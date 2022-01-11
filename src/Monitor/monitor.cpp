//------------------------------------------------------------------------------
// File:   monitor.cpp
// Authors:Pablo López Mosqueda
//         Leonor Murphy Anía
// Date:   18/12/2021
// Coms:   Implementación del monitor que emplea gestorDeColas
//------------------------------------------------------------------------------

#include <iostream>
#include <mutex>
#include <condition_variable>
#include "monitor.hpp"

using namespace std;

ControldeCola::ControldeCola(){ //constructor
    numElementos = 0;
    fin = false;
}

template <class T>
void ControldeCola::escribirCola(BoundedQueue<T>& cola, T elemento){
    unique_lock<mutex> lck(mtxMonitor);

    while(numElementos >= MAX_ELEMENTOS){
        estaEscribiendo.wait(lck);
    }
    
    cola.enqueue(elemento);
    numElementos ++;
    estaBorrando.notify_one();
}

template <class T>
bool ControldeCola::leerCola(BoundedQueue<T>& cola, T& elemento){
    unique_lock<mutex> lck(mtxMonitor);
    bool anyadido = false;
    while(numElementos <= 0 && !fin){
        estaBorrando.wait(lck);
    }
    if(numElementos > 0) {
        numElementos--;
        elemento = cola.first(); //guardar el elemento antes de desencolarlo
        cola.dequeue();
        estaEscribiendo.notify_one();
        anyadido = true;  
    }
    return anyadido;
}

void ControldeCola::finalizar() {
    unique_lock<mutex> lck(mtxMonitor);
    fin = true;
    estaBorrando.notify_all();
}