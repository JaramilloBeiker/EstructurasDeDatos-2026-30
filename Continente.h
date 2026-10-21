#ifndef CONTINENTE_H
#define CONTINENTE_H

#include "Territorio.h"
#include <list>
#include <string>

using namespace std;

class Jugador;

class Continente {
private:
    string nombre;
    list<Territorio*> territorios;
    list<Continente*> continentesAdyacentes;

public:
    Continente(const string& nombre);

    void AgregarTerritorio(Territorio* territorio);
    void AgregarVecino(Continente* vecino);
    bool VerificarVecino(const Continente* vecino) const;
    bool EstaControladoPor(const Jugador* jugador) const;
    int ObtenerBonificacion() const;

    const string& ObtenerNombre() const;
    const list<Territorio*>& ObtenerTerritorios() const;
    const list<Continente*>& ObtenerContinentesAdyacentes() const;
};

#endif