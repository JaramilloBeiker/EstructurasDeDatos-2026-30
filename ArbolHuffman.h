#ifndef ARBOL_HUFFMAN_H
#define ARBOL_HUFFMAN_H

#include "Nodo.h"
#include <map>
#include <string>

using namespace std;

class ArbolHuffman {
private:
    Nodo* raiz;
    map<char, string> diccionario;

    void ConstruirDiccionario(Nodo* nodo, const string& codigo);

public:
    ArbolHuffman(const map<char, long long>& frecuencias);
    ~ArbolHuffman();

    const map<char, string>& ObtenerDiccionario() const;
    Nodo* ObtenerRaiz() const;
};

#endif
