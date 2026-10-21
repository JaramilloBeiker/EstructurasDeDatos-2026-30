#ifndef ARBOL_HUFFMAN_HPP
#define ARBOL_HUFFMAN_HPP

#include "ArbolHuffman.h"
#include "Nodo.hpp"
#include <vector>

ArbolHuffman::ArbolHuffman(const map<char, long long>& frecuencias) {
    raiz = 0;
    vector<Nodo*> nodos;

    map<char, long long>::const_iterator frecuencia;
    for (frecuencia = frecuencias.begin(); frecuencia != frecuencias.end(); ++frecuencia) {
        nodos.push_back(new Nodo(frecuencia->first, frecuencia->second));
    }

    while (nodos.size() > 1) {
        int posicionMenor = 0;
        for (int i = 1; i < static_cast<int>(nodos.size()); ++i) {
            if (nodos[i]->ObtenerFrecuencia() < nodos[posicionMenor]->ObtenerFrecuencia()) {
                posicionMenor = i;
            }
        }
        Nodo* hijoIzquierdo = nodos[posicionMenor];
        nodos.erase(nodos.begin() + posicionMenor);

        posicionMenor = 0;
        for (int i = 1; i < static_cast<int>(nodos.size()); ++i) {
            if (nodos[i]->ObtenerFrecuencia() < nodos[posicionMenor]->ObtenerFrecuencia()) {
                posicionMenor = i;
            }
        }
        Nodo* hijoDerecho = nodos[posicionMenor];
        nodos.erase(nodos.begin() + posicionMenor);

        nodos.push_back(new Nodo(hijoIzquierdo, hijoDerecho));
    }

    if (!nodos.empty()) {
        raiz = nodos[0];
        ConstruirDiccionario(raiz, "");
    }
}

ArbolHuffman::~ArbolHuffman() {
    delete raiz;
}

void ArbolHuffman::ConstruirDiccionario(Nodo* nodo, const string& codigo) {
    if (nodo == 0) {
        return;
    }

    if (nodo->EsHoja()) {
        string codigoHoja = codigo;
        if (codigoHoja.empty()) {
            codigoHoja = "0";
        }
        diccionario[nodo->ObtenerSimbolo()] = codigoHoja;
        return;
    }

    ConstruirDiccionario(nodo->ObtenerIzquierdo(), codigo + "0");
    ConstruirDiccionario(nodo->ObtenerDerecho(), codigo + "1");
}

const map<char, string>& ArbolHuffman::ObtenerDiccionario() const {
    return diccionario;
}

Nodo* ArbolHuffman::ObtenerRaiz() const {
    return raiz;
}

#endif
