#ifndef NODO_H
#define NODO_H

#include <string>

using namespace std;

class Nodo {
private:
    char simbolo;
    bool tieneSimbolo;
    long long frecuencia;
    Nodo* izquierdo;
    Nodo* derecho;

public:
    Nodo(char simbolo, long long frecuencia);
    Nodo(Nodo* nodoIzquierdo, Nodo* nodoDerecho);
    ~Nodo();

    bool EsHoja() const;
    char ObtenerSimbolo() const;
    long long ObtenerFrecuencia() const;
    Nodo* ObtenerIzquierdo() const;
    Nodo* ObtenerDerecho() const;
};

#endif
