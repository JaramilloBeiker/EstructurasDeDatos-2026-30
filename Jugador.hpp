#include "Jugador.h"
#include "Territorio.h"

Jugador::Jugador(const string& nombre, const string& color) {
    this->nombre = nombre;
    this->color = color;
    this->ejercito = 0;
}

void Jugador::AgregarTerritorio(Territorio* territorio) {
    territorios.push_back(territorio);
}

void Jugador::EliminarTerritorio(Territorio* territorio) {
    territorios.remove(territorio);
}

void Jugador::AgregarEjercito(int cantidad) {
    ejercito += cantidad;
}

void Jugador::EliminarEjercito(int cantidad) {
    ejercito -= cantidad;
    if (ejercito < 0) {
        ejercito = 0;
    }
}

// Getters con sus constantes y referencias exactas
const string& Jugador::ObtenerNombre() const {
    return nombre;
}

const string& Jugador::ObtenerColor() const {
    return color;
}

int Jugador::ObtenerEjercito() const {
    return ejercito;
}

const list<Territorio*>& Jugador::ObtenerTerritorios() const {
    return territorios;
}

const vector<string>& Jugador::ObtenerCartas() const {
    return cartas;
}
