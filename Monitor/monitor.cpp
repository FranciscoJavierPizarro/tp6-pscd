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
#include "../BoundedQueue/BoundedQueue.hpp"
#include "monitor.hpp"

using namespace std;

ControldeCola::ControldeCola(){ //constructor
    numElementos = 0;
}

template <class T>
void ControldeCola::escribirCola(BoundedQueue<T> cola, T elemento){
    unique_lock<mutex> lck(mtxMonitor);

    while(numElementos >= MAX_ELEMENTOS){
        estaEscribiendo.wait(lck);
    }
    
    cola.enqueue(elemento);
    numElementos ++;
    estaBorrando.notify_one();
}

template <class T>
void ControldeCola::leerCola(BoundedQueue<T> cola, T elemento){
    unique_lock<mutex> lck(mtxMonitor);

    while(numElementos <= 0){
        estaBorrando.wait(lck);
    }

    elemento = cola.first(); //guardar el elemento antes de desencolarlo
    cola.dequeue();
    numElementos--;
    estaEscribiendo.notify_one();  
}
