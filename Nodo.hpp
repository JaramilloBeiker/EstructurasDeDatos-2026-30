#ifndef NODO_HPP
#define NODO_HPP

#include "Nodo.h"

Nodo::Nodo(char simbolo, long long frecuencia) {
    this->simbolo = simbolo;
    tieneSimbolo = true;
    this->frecuencia = frecuencia;
    izquierdo = 0;
    derecho = 0;
}

Nodo::Nodo(Nodo* nodoIzquierdo, Nodo* nodoDerecho) {
    simbolo = 0;
    tieneSimbolo = false;
    frecuencia = 0;
    izquierdo = nodoIzquierdo;
    derecho = nodoDerecho;

    if (nodoIzquierdo != 0) {
        frecuencia += nodoIzquierdo->ObtenerFrecuencia();
    }
    if (nodoDerecho != 0) {
        frecuencia += nodoDerecho->ObtenerFrecuencia();
    }
}

Nodo::~Nodo() {
    delete izquierdo;
    delete derecho;
}

bool Nodo::EsHoja() const {
    return tieneSimbolo;
}

char Nodo::ObtenerSimbolo() const {
    return simbolo;
}

long long Nodo::ObtenerFrecuencia() const {
    return frecuencia;
}

Nodo* Nodo::ObtenerIzquierdo() const {
    return izquierdo;
}

Nodo* Nodo::ObtenerDerecho() const {
    return derecho;
}

#endif
