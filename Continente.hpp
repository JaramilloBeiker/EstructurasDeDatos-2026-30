#ifndef CONTINENTE_HPP
#define CONTINENTE_HPP

#include "Continente.h"
#include "Jugador.h"

using namespace std;

Continente::Continente(const string& nombre) {
    this->nombre = nombre;
}

void Continente::AgregarTerritorio(Territorio* territorio) {
    if (territorio == nullptr || territorio->ObtenerContinente() != nombre) {
        return;
    }

    for (Territorio* existente : territorios) {
        if (existente->ObtenerCodigo() == territorio->ObtenerCodigo()) {
            return;
        }
    }

    territorios.push_back(territorio);
}

void Continente::AgregarVecino(Continente* vecino) {
    if (vecino == nullptr || vecino == this) {
        return;
    }

    for (Continente* existente : continentesAdyacentes) {
        if (existente == vecino || existente->ObtenerNombre() == vecino->ObtenerNombre()) {
            return;
        }
    }

    continentesAdyacentes.push_back(vecino);
}

bool Continente::VerificarVecino(const Continente* vecino) const {
    if (vecino == nullptr || vecino == this) {
        return false;
    }

    for (Continente* existente : continentesAdyacentes) {
        if (existente == vecino || existente->ObtenerNombre() == vecino->ObtenerNombre()) {
            return true;
        }
    }

    return false;
}

bool Continente::EstaControladoPor(const Jugador* jugador) const {
    if (jugador == nullptr || territorios.empty()) {
        return false;
    }

    for (Territorio* territorio : territorios) {
        if (territorio == nullptr || territorio->ObtenerDueno() != jugador) {
            return false;
        }
    }

    return true;
}

int Continente::ObtenerBonificacion() const {
    if (nombre == "America del Sur" || nombre == "Australia") {
        return 2;
    }
    if (nombre == "Africa") {
        return 3;
    }
    if (nombre == "America del Norte" || nombre == "Europa") {
        return 5;
    }
    if (nombre == "Asia") {
        return 7;
    }
    return 0;
}

const string& Continente::ObtenerNombre() const {
    return nombre;
}

const list<Territorio*>& Continente::ObtenerTerritorios() const {
    return territorios;
}

const list<Continente*>& Continente::ObtenerContinentesAdyacentes() const {
    return continentesAdyacentes;
}
#endif
