//------------------------------------------------------------------------------
// File:   monitor.hpp
// Authors:Pablo López Mosqueda
//         Leonor Murphy Anía
// Date:   18/12/2021
// Coms:   Interfaz y definición del monitor que emplea gestorDeColas
//------------------------------------------------------------------------------
#ifndef MONITOR_H
#define MONITOR_H

#include <mutex>
#include <condition_variable>
#include <iostream>
#include "../BoundedQueue/BoundedQueue.hpp"

using namespace std;

const int MAX_ELEMENTOS = 50; //tamaño máximo de las colas

class ControldeCola {
    public:
        ControldeCola();
        template <class T> bool leerCola(BoundedQueue<T>& cola, T& elemento);//borra elementos de la cola
        template <class T> void escribirCola(BoundedQueue<T>& cola, T elemento);//añade elementos a la cola
        void finalizar();//indica que la cola no va a tener mas datos nuevos
    private:
        int numElementos;
        bool fin;
        mutex mtxMonitor;
        condition_variable estaEscribiendo;
        condition_variable estaBorrando;
};
#include "monitor.cpp"
#endif