//------------------------------------------------------------------------------
// File:   inform.h
// Authors: Inés Román Gracia
// Axel Isaac Pazmiño Ortega
// Alicia Lázaro Huerta
// Francisco Javier Pizarro Martínez
// Date:   15/01/2022
//------------------------------------------------------------------------------


#ifndef INFORM_H
#define INFORM_H
#include <iostream>
using namespace std;

// Pre-declaraciones:
/* Interfaz del TAD programas. Un programa está definido por cuatro parámetros: una cadena nombre,
que identifica a cada programa y es único para cada programa; la fecha fecha, que indica el momento de la
creación del programa; la cadena lenguaje, que indica en qué lenguaje de progrmación está escrito dicho programa;
y la cadena su descripción, en la que se indican algunos detalles acerca del programa */
struct inform;
/* Crea un progarma a partir de un nombre, una fecha, un lenguaje y una descripción */
void crear(string nom, int nAp, inform& p);
/* Devuelve la cadena nombre de un inform */
string identificador(const inform& p);
/* Devuelve la fecha de creación de un inform */
int valor(const inform& p);
/* Devuelve la cadena lenguaje de un inform */
void aumentarValor(inform& p);

bool operator>(const inform& p1, const inform& p2);
// Declaración:
struct inform{
    friend void crear(string nom, int nAp, inform& p);
    friend string identificador(const inform& p);
    friend int valor(const inform& p);
    friend void aumentarValor(inform& p);
    friend bool operator>(const inform& p1, const inform& p2);
    private:
    // Representación de valores del TAD:
    string ident;
    int val;
};
#endif