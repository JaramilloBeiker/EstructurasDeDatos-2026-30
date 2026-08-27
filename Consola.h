#ifndef CONSOLA_H
#define CONSOLA_H

#include "Juego.h"
#include "Consola.hpp"
#include <string>
#include <vector>

using namespace std;

// Funciones auxiliares de la consola
bool esNumeroEntero(const string& cadena);
bool esNombreJugadorValido(const string& nombre);
vector<string> separarPalabras(const string& linea);

// Prototipos de la consola
void procesarEntrada(const vector<string>& palabras_comando, bool& ejecutando, Juego& juego);
void iniciarConsola();

#endif