//*****************************************************************
// File:   LinkedList.hpp
// Authors:Francisco Javier Pizarro Martínez
//
// Date:   08/01/2022
// Coms:   Especificación de una lista enlazada acotada.
//*****************************************************************

#ifndef LINKED_LIST
#define LINKED_LIST

#include <iostream>
#include <string>

using namespace std;

template <class T>
class LinkedList {
public:
    //-----------------------------------------------------
    //Pre:  0<N
    //Post: this=< > AND #this=N
    //Com:  constructor
    LinkedList(const int N);
    //-----------------------------------------------------
    //Pre:
    //Post:
    //Com:  destructor
    ~LinkedList();
    //-----------------------------------------------------
    //Pre:
    //Post: this=<>
    void empty();
    //-----------------------------------------------------
    //Pre:  this=< d_1 ... d_n > AND n<#this
    //Post: this=< d_1 ... d_n d > 
    void add(const T d);
    //-----------------------------------------------------
    //Post: devuelve si d € a la cola
    bool esta(const T d);
    //-----------------------------------------------------
    //Pre:  this=< d_1 ... d_n >
    //Post: length()=n
    int length();
    //-----------------------------------------------------
private:
    int N;
    int nEl;
    T *data;
};
// Para poder trabajar con tipos genéricos, declaración y código deben estar juntos
#include "LinkedList.cpp"
#endif