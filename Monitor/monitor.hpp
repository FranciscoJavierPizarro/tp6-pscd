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
        template <class T> void leerCola(BoundedQueue<T> cola, T elemento);
        template <class T> void escribirCola(BoundedQueue<T> cola, T elemento);

    private:
        int numElementos;
        mutex mtxMonitor;
        condition_variable estaEscribiendo;
        condition_variable estaBorrando;
};

#endif