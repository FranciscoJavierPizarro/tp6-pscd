//*****************************************************************
// File:   LinkedList.cpp
// Authors:Francisco Javier Pizarro Martínez
//
// Date:   08/01/2022
// Coms:   Implemetación de la clase linked list
//*****************************************************************

#include "LinkedList.hpp"
#include <cassert>
#include <cstring>

//-----------------------------------------------------
template <class T>
LinkedList<T>::LinkedList(const int N) {

    this->N = N;
    data = new T[N];
    empty();
}
//-----------------------------------------------------
template <class T>
void LinkedList<T>::empty() {
    nEl = 0;
}
//-----------------------------------------------------
template <class T>
LinkedList<T>::~LinkedList() {

    delete[] data;
}
//-----------------------------------------------------
template <class T>
void LinkedList<T>::add(const T d) {

    assert(nEl<N);
    if(!esta(d)) {
        data[nEl] = d;
        nEl++;
    }
}
//-----------------------------------------------------
template <class T>
bool LinkedList<T>::esta(const T d) {
    for(int i = 0; i < nEl; i++) if(data[nEl] == d) return true;
    return false;
}
//-----------------------------------------------------
template <class T>
int LinkedList<T>::length() {
    
    return nEl;
}
//-----------------------------------------------------