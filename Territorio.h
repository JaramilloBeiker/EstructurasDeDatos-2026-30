#ifndef TERRITORIO_H
#define TERRITORIO_H

#include <string>
#include <list>

using namespace std;

class Jugador; //Podemos usar una declaracion adelantada, no necesitamos conocer todo el TAD jugador, solo necesitamos saber que existe para apuntar

class Territorio {
    private:
        string nombre; //Indica el nombre del territorio
        string codigo; //Indica el codigo del territorio
        string continente; //Indica el continente al que pertenece el territorio
        Jugador* dueno; //Indica el jugador que posee el territorio
        int unidades; //Indica la cantidad de unidades que estan en el territorio
        list<Territorio*> territoriosAdyacentes; //Indica los territorios que son vecinos del territorio actual, es decir, aquellos territorios que se conectan con el territorio actual

    public:

        Territorio(const string& nombre, const string& codigo, const string& continente, Jugador* Dueno, int unidades);
        
        void AgregarUnidades(int cantidad);
        void EliminarUnidades(int cantidad);
        void CambiarDueno(Jugador* nuevoDueno);
        void AgregarVecino(Territorio* vecino);
        bool VerificarVecino(Territorio* vecino) const;
        // Getters
        const string& ObtenerNombre() const;
        const string& ObtenerContinente() const;
        const string& ObtenerCodigo() const;
        int ObtenerUnidades() const;
        Jugador* ObtenerDueno() const;
        const list<Territorio*>& ObtenerTerritoriosAdyacentes() const;

};

#endif