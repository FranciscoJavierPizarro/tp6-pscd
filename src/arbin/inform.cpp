//------------------------------------------------------------------------------
// File:   inform.cpp
// Authors: Axel Isaac Pazmiño Ortega, Alicia Lázaro Huerta y Francisco Javier Pizarro Martínez
// Date:   15/01/2022
//------------------------------------------------------------------------------


#include "inform.h"

void crear(string nom, int nAp, inform& p){
    p.ident = nom;
    p.val = nAp;
}

string identificador(const inform& p){
    return p.ident;
}

int valor(const inform& p){
    return p.val;
}

void aumentarValor(inform& p){
    p.val++;
}

bool operator>(const inform& p1, const inform& p2){
    return p1.val > p2.val;
}