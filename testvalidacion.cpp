#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <ctime>
#include <functional>
#include <sstream>

// Permitimos preparar estados internos exclusivamente desde este archivo de pruebas.
#define private public
#include "Juego.hpp"
#undef private
#include "Jugador.hpp"
#include "Territorio.hpp"

using namespace std;

// --- UTILIDADES PARA LA SUITE DE PRUEBAS ---

const vector<string> idsTerritorios = {
    "1.1", "1.2", "1.3", "1.4", "1.5", "1.6", "1.7", "1.8", "1.9",
    "2.1", "2.2", "2.3", "2.4",
    "3.1", "3.2", "3.3", "3.4", "3.5", "3.6", "3.7",
    "4.1", "4.2", "4.3", "4.4", "4.5", "4.6",
    "5.1", "5.2", "5.3", "5.4", "5.5", "5.6", "5.7", "5.8", "5.9", "5.10", "5.11", "5.12",
    "6.1", "6.2", "6.3", "6.4"
};

// Crea un archivo de prueba que el codigo de InicializarJuego pueda leer correctamente
void crearArchivoPrueba(const string& nombreArchivo) {
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo crear " << nombreArchivo << endl;
        return;
    }

    // Definimos 3 jugadores (minimo requerido por tu codigo)
    archivo << "3" << endl;
    archivo << "daniel Rojo" << endl;
    archivo << "beiker Azul" << endl;
    archivo << "sarahy Verde" << endl;

    // Lista de los 42 IDs
    // IMPORTANTE: Tu codigo hace: archivoEntrada >> codigoTerritorio >> color >> unidades
    // Por lo tanto, el archivo SOLO debe tener 3 datos por linea.
    for (int i = 0; i < 42; i++) {
        string id = idsTerritorios[i];
        string color = (i % 3 == 0) ? "Rojo" : (i % 3 == 1 ? "Azul" : "Verde");
        int unidades = i < 3 ? 35 : 0;
        
        // Formato exacto: ID Color Unidades
        archivo << id << " " << color << " " << unidades << endl;
    }
    archivo.close();
}

void imprimirCabecera(string titulo) {
    cout << "\n\n====================================================================" << endl;
    cout << "  " << titulo << endl;
    cout << "====================================================================\n\n";
}


//funcion para separar cada titulo de prueba y que se vea mejor en la consola
void imprimirSeparadorPrueba(string titulo) {
    cout << "\n\n-------------------- " << titulo << " --------------------\n\n";
}

void mostrarComando(const string& comando) {
    cout << "Comando automatico: " << comando << endl;
}

void prepararPrueba(string titulo, const string& nombreArchivo) {
    crearArchivoPrueba(nombreArchivo);
    imprimirSeparadorPrueba(titulo);
}

void ejecutarConEntrada(const string& entrada, const function<void()>& prueba) {
    istringstream valores(entrada);
    string valor;
    cout << "Entrada automatica:" << endl;
    while (getline(valores, valor)) {
        if (!valor.empty()) {
            cout << "> " << valor << endl;
        }
    }

    istringstream entradaPrueba(entrada);
    streambuf* entradaOriginal = cin.rdbuf(entradaPrueba.rdbuf());
    prueba();
    cin.rdbuf(entradaOriginal);
}

// --- EJECUCION DE PRUEBAS ---

int main() {
    srand(time(0));

    // --- PARTE 1: VALIDACIONES ---
    imprimirCabecera("PRUEBAS DE VALIDACION (V1-V10)");

    // V1: Juego no inicializado
    Juego j1;
    imprimirSeparadorPrueba("V1: Juego no inicializado");
    mostrarComando("AtacarTerritorio(\"daniel\", \"1.2\")");
    j1.AtacarTerritorio("daniel", "1.2");

    // V2: Juego ya terminado
    prepararPrueba("V2: Juego ya terminado", "archivo_validaciones.txt");
    Juego j2;
    mostrarComando("InicializarJuego(\"archivo_validaciones.txt\")");
    j2.InicializarJuego("archivo_validaciones.txt");
    j2.juegoTerminado = true;
    mostrarComando("AtacarTerritorio(\"daniel\", \"1.2\")");
    j2.AtacarTerritorio("daniel", "1.2");

    // V3: Jugador no valido
    prepararPrueba("V3: Jugador no valido", "archivo_validaciones.txt");
    Juego j3;
    mostrarComando("InicializarJuego(\"archivo_validaciones.txt\")");
    j3.InicializarJuego("archivo_validaciones.txt");
    mostrarComando("AtacarTerritorio(\"Fantasma\", \"1.2\")");
    j3.AtacarTerritorio("Fantasma", "1.2");

    // V4: Unidades no reclamadas
    prepararPrueba("V4: Unidades no reclamadas", "archivo_validaciones.txt");
    Juego j4;
    mostrarComando("InicializarJuego(\"archivo_validaciones.txt\")");
    j4.InicializarJuego("archivo_validaciones.txt");
    mostrarComando("AtacarTerritorio(\"daniel\", \"1.2\")");
    j4.AtacarTerritorio("daniel", "1.2");

    // V5: No es el turno
    prepararPrueba("V5: Turno invalido", "archivo_validaciones.txt");
    Juego j5;
    mostrarComando("InicializarJuego(\"archivo_validaciones.txt\")");
    j5.InicializarJuego("archivo_validaciones.txt");
    mostrarComando("ReclamarUnidades(\"daniel\") con entrada 1.1, 4");
    ejecutarConEntrada("1.1\n4\n", [&]() { j5.ReclamarUnidades("daniel"); });
    mostrarComando("AtacarTerritorio(\"beiker\", \"1.2\")");
    j5.AtacarTerritorio("beiker", "1.2");

    // V6: Territorio no existe
    prepararPrueba("V6: Territorio no existe", "archivo_validaciones.txt");
    Juego j6;
    mostrarComando("InicializarJuego(\"archivo_validaciones.txt\")");
    j6.InicializarJuego("archivo_validaciones.txt");
    mostrarComando("ReclamarUnidades(\"daniel\") con entrada 1.1, 4");
    ejecutarConEntrada("1.1\n4\n", [&]() { j6.ReclamarUnidades("daniel"); });
    mostrarComando("AtacarTerritorio(\"daniel\", \"99.99\") con origen Alaska");
    ejecutarConEntrada("Alaska\n", [&]() { j6.AtacarTerritorio("daniel", "99.99"); });

    // V7: Territorio no propio
    prepararPrueba("V7: Territorio no propio", "archivo_validaciones.txt");
    Juego j7;
    mostrarComando("InicializarJuego(\"archivo_validaciones.txt\")");
    j7.InicializarJuego("archivo_validaciones.txt");
    mostrarComando("ReclamarUnidades(\"daniel\") con entrada 1.1, 4");
    ejecutarConEntrada("1.1\n4\n", [&]() { j7.ReclamarUnidades("daniel"); });
    mostrarComando("AtacarTerritorio(\"daniel\", \"1.2\") con origen Alberta");
    ejecutarConEntrada("Alberta\n", [&]() { j7.AtacarTerritorio("daniel", "1.2"); });

    // V8: Unidades insuficientes
    const string archivoV8 = "archivo_v8.txt";
    prepararPrueba("V8: Unidades insuficientes", archivoV8);
    ofstream f(archivoV8);
    f << "3\ndaniel Rojo\nbeiker Azul\nsarahy Verde\n";
    for (int i = 0; i < 42; i++) {
        string color = (i % 3 == 0) ? "Rojo" : (i % 3 == 1 ? "Azul" : "Verde");
        int unidades = (i == 0) ? 1 : (i == 3) ? 34 : (i == 1 || i == 2) ? 35 : 0;
        f << idsTerritorios[i] << " " << color << " " << unidades << endl;
    }
    f.close();

    Juego j8;
    mostrarComando("InicializarJuego(\"archivo_v8.txt\")");
    j8.InicializarJuego(archivoV8);
    mostrarComando("ReclamarUnidades(\"daniel\") con entrada 1.4, 4");
    ejecutarConEntrada("1.4\n4\n", [&]() { j8.ReclamarUnidades("daniel"); });
    mostrarComando("AtacarTerritorio(\"daniel\", \"1.2\") con origen Alaska");
    ejecutarConEntrada("Alaska\n", [&]() { j8.AtacarTerritorio("daniel", "1.2"); });

    // V9: Territorio ya propio
    prepararPrueba("V9: Territorio ya propio", "archivo_validaciones.txt");
    Juego j9;
    mostrarComando("InicializarJuego(\"archivo_validaciones.txt\")");
    j9.InicializarJuego("archivo_validaciones.txt");
    mostrarComando("ReclamarUnidades(\"daniel\") con entrada 1.1, 4");
    ejecutarConEntrada("1.1\n4\n", [&]() { j9.ReclamarUnidades("daniel"); });
    mostrarComando("AtacarTerritorio(\"daniel\", \"1.1\") con origen Alaska");
    ejecutarConEntrada("Alaska\n", [&]() { j9.AtacarTerritorio("daniel", "1.1"); });

    // V10: No adyacentes
    prepararPrueba("V10: No adyacentes", "archivo_validaciones.txt");
    Juego j10;
    mostrarComando("InicializarJuego(\"archivo_validaciones.txt\")");
    j10.InicializarJuego("archivo_validaciones.txt");
    mostrarComando("ReclamarUnidades(\"daniel\") con entrada 1.1, 4");
    ejecutarConEntrada("1.1\n4\n", [&]() { j10.ReclamarUnidades("daniel"); });
    mostrarComando("AtacarTerritorio(\"daniel\", \"6.4\") con origen Alaska");
    ejecutarConEntrada("Alaska\n", [&]() { j10.AtacarTerritorio("daniel", "6.4"); });

    cout << "\n--- FIN DE LAS PRUEBAS DE VERIFICACION ---" << endl;
    return 0;
}
