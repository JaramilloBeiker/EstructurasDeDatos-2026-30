#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <cstdlib>

std::vector<int> dadosForzados;
size_t indiceDadoForzado = 0;

namespace std {
int randPrueba() {
    if (::indiceDadoForzado < ::dadosForzados.size()) {
        return ::dadosForzados[::indiceDadoForzado++] - 1;
    }
    return 0;
}
}

#define private public
#define rand randPrueba
#include "Juego.hpp"
#undef rand
#undef private
#include "Jugador.hpp"
#include "Territorio.hpp"

using namespace std;

const vector<string> idsTerritorios = {
    "1.1", "1.2", "1.3", "1.4", "1.5", "1.6", "1.7", "1.8", "1.9",
    "2.1", "2.2", "2.3", "2.4",
    "3.1", "3.2", "3.3", "3.4", "3.5", "3.6", "3.7",
    "4.1", "4.2", "4.3", "4.4", "4.5", "4.6",
    "5.1", "5.2", "5.3", "5.4", "5.5", "5.6", "5.7", "5.8", "5.9", "5.10", "5.11", "5.12",
    "6.1", "6.2", "6.3", "6.4"
};

void crearArchivoAtaque(const string& archivo, int unidadesOrigen, int unidadesDestino) {
    ofstream salida(archivo);
    salida << "3\n";
    salida << "daniel Rojo\n";
    salida << "beiker Azul\n";
    salida << "sarahy Verde\n";

    for (size_t i = 0; i < idsTerritorios.size(); i++) {
        string color = (i % 3 == 0) ? "Rojo" : (i % 3 == 1 ? "Azul" : "Verde");
        int unidades = 0;

        if (i == 0) {
            unidades = unidadesOrigen;
        } else if (i == 1) {
            unidades = unidadesDestino;
        } else if (i == 3) {
            unidades = 35 - unidadesOrigen;
        } else if (i == 4) {
            unidades = 35 - unidadesDestino;
        } else if (i == 2) {
            unidades = 35;
        }

        salida << idsTerritorios[i] << " " << color << " " << unidades << "\n";
    }
}

void separador(const string& titulo) {
    cout << "\n\n";
    cout << "============================================================\n";
    cout << "  " << titulo << "\n";
    cout << "============================================================\n";
}

void comando(const string& texto) {
    cout << "[COMANDO] " << texto << endl;
}

void forzarDados(const vector<int>& dados) {
    dadosForzados = dados;
    indiceDadoForzado = 0;
    cout << "[DADOS DE PRUEBA] ";
    for (int dado : dados) {
        cout << dado << " ";
    }
    cout << endl;
}

void ejecutarConEntrada(const string& entrada, const function<void()>& operacion) {
    cout << "[ENTRADA AUTOMATICA]" << endl;
    istringstream valores(entrada);
    string valor;
    while (getline(valores, valor)) {
        if (!valor.empty()) {
            cout << "  > " << valor << endl;
        }
    }

    istringstream entradaPrueba(entrada);
    streambuf* entradaOriginal = cin.rdbuf(entradaPrueba.rdbuf());
    operacion();
    cin.rdbuf(entradaOriginal);
}

void prepararJuego(Juego& juego, const string& archivo) {
    cout << "Preparacion: inicializar y reclamar unidades" << endl;
    comando("InicializarJuego(\"" + archivo + "\")");
    juego.InicializarJuego(archivo);
    comando("ReclamarUnidades(\"daniel\") con entrada 1.1, 4");
    ejecutarConEntrada("1.1\n4\n", [&]() { juego.ReclamarUnidades("daniel"); });
}

void mostrarTropasFinales(Juego& juego) {
    Territorio* origen = juego.BuscarTerritorio("1.1");
    Territorio* destino = juego.BuscarTerritorio("1.2");
    cout << "[RESUMEN] Tropas finales: "
         << origen->ObtenerNombre() << " = " << origen->ObtenerUnidades()
         << " | " << destino->ObtenerNombre() << " = " << destino->ObtenerUnidades()
         << endl;
}

void prepararEliminacion(Juego& juego) {
    Jugador* daniel = juego.BuscarJugador("daniel");
    Jugador* beiker = juego.BuscarJugador("beiker");
    Territorio* objetivo = juego.BuscarTerritorio("1.2");

    for (Territorio* territorio : juego.territorios) {
        if (territorio != objetivo && territorio->ObtenerDueno() == beiker) {
            beiker->EliminarTerritorio(territorio);
            territorio->CambiarDueno(daniel);
            daniel->AgregarTerritorio(territorio);
        }
    }

    objetivo->unidades = 1;
}

void prepararVictoria(Juego& juego) {
    Jugador* daniel = juego.BuscarJugador("daniel");
    Territorio* objetivo = juego.BuscarTerritorio("6.4");

    for (Territorio* territorio : juego.territorios) {
        Jugador* duenoAnterior = territorio->ObtenerDueno();
        if (territorio != objetivo && duenoAnterior != daniel) {
            duenoAnterior->EliminarTerritorio(territorio);
            territorio->CambiarDueno(daniel);
            daniel->AgregarTerritorio(territorio);
        }
    }

    juego.BuscarTerritorio("6.2")->unidades = 10;
    objetivo->unidades = 1;
}

int main() {
    srand(1);

    const string archivoE1 = "archivo_e1.txt";
    crearArchivoAtaque(archivoE1, 10, 20);
    Juego e1;
    separador("E1: Ataque basico sin conquista - gana el atacante");
    prepararJuego(e1, archivoE1);
    forzarDados({6, 6, 6, 1, 1});
    comando("AtacarTerritorio(\"daniel\", \"1.2\") con origen Alaska y respuesta n");
    ejecutarConEntrada("Alaska\nn\n", [&]() { e1.AtacarTerritorio("daniel", "1.2"); });
    mostrarTropasFinales(e1);
    cout << "Fin de E1." << endl;

    const string archivoE2 = "archivo_e2.txt";
    crearArchivoAtaque(archivoE2, 10, 20);
    Juego e2;
    separador("E2: Ataque basico sin conquista - gana el defensor");
    prepararJuego(e2, archivoE2);
    forzarDados({1, 1, 1, 6, 6});
    comando("AtacarTerritorio(\"daniel\", \"1.2\") con origen Alaska y respuesta n");
    ejecutarConEntrada("Alaska\nn\n", [&]() { e2.AtacarTerritorio("daniel", "1.2"); });
    mostrarTropasFinales(e2);
    cout << "Fin de E2." << endl;

    const string archivoE3 = "archivo_e3.txt";
    crearArchivoAtaque(archivoE3, 10, 1);
    Juego e3;
    separador("E3: Conquista de territorio");
    prepararJuego(e3, archivoE3);
    forzarDados({6, 6, 6, 1});
    comando("AtacarTerritorio(\"daniel\", \"1.2\") hasta conquistar y mover 2 tropas");
    ejecutarConEntrada("Alaska\n2\n", [&]() { e3.AtacarTerritorio("daniel", "1.2"); });
        cout << "[RESUMEN] Propietario final de Alberta: "
            << e3.BuscarTerritorio("1.2")->ObtenerDueno()->ObtenerNombre()
             << endl;

    const string archivoE4 = "archivo_e4.txt";
    crearArchivoAtaque(archivoE4, 0, 1);
    Juego e4;
    separador("E4: Conquista con movimiento de unidades");
    prepararJuego(e4, archivoE4);
    forzarDados({6, 6, 6, 1});
    comando("AtacarTerritorio(\"daniel\", \"1.2\") con origen Alaska y mover 2 tropas");
    ejecutarConEntrada("Alaska\n2\n", [&]() { e4.AtacarTerritorio("daniel", "1.2"); });
            cout << "[RESUMEN] Unidades finales en Alaska: "
                << e4.BuscarTerritorio("1.1")->ObtenerUnidades() << endl;
            cout << "[RESUMEN] Unidades finales en Alberta: "
                << e4.BuscarTerritorio("1.2")->ObtenerUnidades() << endl;

    const string archivoE5 = "archivo_e5.txt";
    crearArchivoAtaque(archivoE5, 10, 20);
    Juego e5;
    separador("E5: Continuar ataque");
    prepararJuego(e5, archivoE5);
    forzarDados({6, 6, 6, 1, 6, 6, 6, 1});
    comando("AtacarTerritorio(\"daniel\", \"1.2\") con respuestas s y n");
    ejecutarConEntrada("Alaska\ns\nn\n", [&]() { e5.AtacarTerritorio("daniel", "1.2"); });
    mostrarTropasFinales(e5);
    cout << "Fin de E5." << endl;

    const string archivoE6 = "archivo_e6.txt";
    crearArchivoAtaque(archivoE6, 10, 1);
    Juego e6;
    separador("E6: Eliminacion del jugador defensor");
    prepararJuego(e6, archivoE6);
    prepararEliminacion(e6);
    forzarDados({6, 6, 6, 1});
    comando("AtacarTerritorio(\"daniel\", \"1.2\") para eliminar a beiker y mover 2 tropas");
    ejecutarConEntrada("Alaska\n2\n", [&]() { e6.AtacarTerritorio("daniel", "1.2"); });
        cout << "[RESUMEN] Jugador beiker: "
            << (e6.BuscarJugador("beiker") == nullptr ? "eliminado" : "sigue en la partida")
             << endl;

    const string archivoE7 = "archivo_e7.txt";
    crearArchivoAtaque(archivoE7, 10, 1);
    Juego e7;
    separador("E7: Victoria total");
    prepararJuego(e7, archivoE7);
    prepararVictoria(e7);
    forzarDados({6, 6, 6, 1});
    comando("AtacarTerritorio(\"daniel\", \"6.4\") para conquistar el territorio 42 y mover 2 tropas");
    ejecutarConEntrada("Indonesia\n2\n", [&]() { e7.AtacarTerritorio("daniel", "6.4"); });
            cout << "[RESUMEN] juegoTerminado: "
                << (e7.juegoTerminado ? "true" : "false") << endl;

    cout << "\nFin de las pruebas de ataque E1-E7." << endl;
    return 0;
}
