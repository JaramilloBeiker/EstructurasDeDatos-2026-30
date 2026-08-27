#ifndef JUGADOR_H
#define JUGADOR_H

#include <string>
#include <vector>
#include <list>

using namespace std;

class Territorio; //Podemos usar una declaracion adelantada, no necesitamos conocer todo el TAD terrirtorio, solo necesitamos saber que existe para apuntar

class Jugador {
    private:
        string nombre; //Indica el nombre del jugador
        string color; //Indica el color del jugador
        int ejercito; //Indica la cantidad de ejercito que tiene el jugador
        list<Territorio*> territorios; //Indica los territorios que posee el jugador
        vector<string> cartas; // Indica las cartas que posee el jugador

    public:
        Jugador(const string& nombre, const string& color);
        
        void AgregarTerritorio(Territorio* territorio);
        void EliminarTerritorio(Territorio* territorio);
        void AgregarEjercito(int cantidad);
        void EliminarEjercito(int cantidad);
        //Getters
        const string& ObtenerNombre() const;
        const string& ObtenerColor() const;
        int ObtenerEjercito() const;
        const list<Territorio*>& ObtenerTerritorios() const;
        const vector<string>& ObtenerCartas() const;
};

#endif