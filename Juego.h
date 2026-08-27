
#ifndef JUEGO_H
#define JUEGO_H

#include "Jugador.h"
#include "Territorio.h"
#include <string>
#include <vector>
#include <list>
using namespace std;

struct DatosTerritorio { //Esta estructura nos va a servir para identificar el nombre y continente cuando lo leamos del archivo de texto
    string codigo;
    string nombre;
    string continente;
};

struct Fronteras
{
    string terrritorio1;
    string territorio2;
};


class Juego {

    private:
        vector<Jugador*> jugadores; //Indica los jugadores en una partida
        list<Territorio*> territorios; //Indica los territorios en una partida
        Jugador* jugadorActual; // Indica que jugador tiene el turno actual
        bool juegoInicializado; //Esta variable la vamos a usar para saber si el juego ya fue iniciado

    public:
        Juego(); //Constructor por defecto
        void InicializarJuego(const string& archivo);
        void AtacarTerritorio(const string& jugador, const string& territorio);
        void EliminarJugadorDeJuego(Jugador* jugadorAEliminar);
        void FortificarTerritorio(const string& jugador, const string& territorio);
        void CambiarTurno(const string& jugador);
        void EstadoJuego();
        bool VerificarGanador();
        Jugador* BuscarJugador(const string& nombre);
        Territorio* BuscarTerritorio(const string& nombre);
        void ConfigurarFronteras();
        void ReclamarUnidades(const string& nombreJugador);

};

#endif