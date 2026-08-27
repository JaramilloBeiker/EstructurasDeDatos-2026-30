#include "Consola.h"
#include "Jugador.hpp"
#include "Territorio.hpp"
#include "Juego.hpp"
#include <iostream>
#include <sstream>
#include <cctype>

using namespace std;

bool esNumeroEntero(const string& cadena) {
    if (cadena.empty()) return false;

    for (char const &c : cadena) {
        if (isdigit(c) == 0) return false;
    }

    return true;
}

bool esNombreJugadorValido(const string& nombre) {
    return (nombre.length() > 0 && nombre.length() <= 8);
}

vector<string> separarPalabras(const string& linea) {
    vector<string> palabras_comando;
    string palabra_individual;
    stringstream ss(linea);

    while (ss >> palabra_individual) {
        palabras_comando.push_back(palabra_individual);
    }
    return palabras_comando;
}

void procesarEntrada(const vector<string>& palabras_comando, bool& ejecutando, Juego& juego) {
    if (palabras_comando.empty()) {
        return;
    }

    string comando_principal = palabras_comando[0];
    int cantidad_palabras = palabras_comando.size();

    if (comando_principal == "ayuda") {
        if (cantidad_palabras == 1) { 
            cout << "Lista de comandos disponibles:" << endl;
            cout << "  - Inicializar" << endl;
            cout << "  - Obtener_unidades" << endl;
            cout << "  - Atacar" << endl;
            cout << "  - Fortificar" << endl;
            cout << "  - Estado_juego" << endl;
            cout << "  - Guardar" << endl;
            cout << "  - Guardar_comprimido" << endl;
            cout << "  - Costo_conquista" << endl;
            cout << "  - Conquista_mas_barata" << endl;
            cout << "  - Salir" << endl;
            cout << "\nPara ver el uso de un comando especifico, escribe: ayuda <comando>" << endl;
        } 
        else if (cantidad_palabras == 2) {
            string subcomando = palabras_comando[1];
            
            if (subcomando == "Inicializar" || subcomando == "inicializar") {
                cout << "Uso: Inicializar <archivo_inicio.txt>" << endl;
                cout << "Inicializa el juego a partir de la información contenida en el archivo \"archivo_inicio.txt\" dejando la interfaz del juego lista para recibir los turnos de cada jugador" << endl;
            } 
            else if (subcomando == "Obtener_unidades" || subcomando == "obtener_unidades") {
                cout << "Uso: Obtener_unidades <nombre_jugador>" << endl;
                cout << "El comando primero informar al jugador cuántas unidades adicionales puede reclamar y luego le pregunta en cuáles de sus territorios las quiere asignar y en qué cantidad." << endl;
            } 
            else if (subcomando == "Atacar" || subcomando == "atacar") {
                cout << "Uso: Atacar <nombre_jugador>" << endl;
                cout << "El comando primero pregunta desde cuál territorio quiere atacar (que contiene actualmente unidades del jugador) y hacia cuál territorio dirigirá el ataque. Luego informa los valores obtenidos con los dados, y la cantidad de unidades que se ganan o pierden. Este proceso se repite hasta que alguno de los dos territorios se quede sin unidades, o hasta que el atacante decida detenerse." << endl;
            } 
            else if (subcomando == "Fortificar" || subcomando == "fortificar") {
                cout << "Uso: Fortificar <nombre_jugador>" << endl;
                cout << "El comando pregunta al jugador los territorios vecinos que desea seleccionar para la fortificación y la cantidad de unidades que se trasladarán de uno al otro." << endl;
            } 
            else if (subcomando == "Estado_juego" || subcomando == "estado_juego") {
                cout << "Uso: Estado_juego" << endl;
                cout << "Presenta en pantalla un resumen de la situacion actual del juego mostrando: " << endl;
                cout << "número de jugadores, nombres y colores de cada uno, jugador con el turno actual y lista de los territorios con el color del jugador que lo controla con la cantidad de unidades que hay ubicadas en cada uno" << endl;
            } 
            else if (subcomando == "Guardar" || subcomando == "guardar") {
                cout << "Uso: Guardar <nombre_archivo>" << endl;
                cout << "Guarda el estado actual del juego en un archivo de texto plano." << endl;
            } 
            else if (subcomando == "Guardar_comprimido" || subcomando == "guardar_comprimido") {
                cout << "Uso: Guardar_comprimido <nombre_archivo>" << endl;
                cout << "Guarda el estado actual del juego en un archivo binario comprimido (.bin)." << endl;
            } 
            else if (subcomando == "Costo_conquista" || subcomando == "costo_conquista") {
                cout << "Uso: Costo_conquista <nombre_jugador> <territorio>" << endl;
                cout << "Calcula el costo y la secuencia de territorios a conquistar para llegar al territorio objetivo dado por el usuario." << endl;
            } 
            else if (subcomando == "Conquista_mas_barata" || subcomando == "conquista_mas_barata") {
                cout << "Uso: Conquista_mas_barata <nombre_jugador>" << endl;
                cout << "De todos los territorios posibles, calcula aquel que pueda implicar un menor número de unidades de ejército perdidas." << endl;
            } 
            else if (subcomando == "Salir" || subcomando == "salir") {
                cout << "Uso: Salir" << endl;
                cout << "Termina la ejecucion de la aplicacion." << endl;
            } 
            else {
                cout << "Error: El comando '" << subcomando << "' no existe." << endl;
            }
        } 
        else {
            cout << "Error en formato. Uso correcto: ayuda o ayuda <comando>" << endl;
        }
    }
    else if (comando_principal == "Inicializar" || comando_principal == "inicializar") {
        if (cantidad_palabras == 2) {
            string nombre_archivo = palabras_comando[1];
            bool es_txt = (nombre_archivo.length() >= 4 && nombre_archivo.substr(nombre_archivo.length() - 4) == ".txt");
            
            if (es_txt) {
                // LLAMADO REAL A JUEGO
                juego.InicializarJuego(nombre_archivo);
            } else {
                cout << "(Archivo no valido) El archivo debe tener extension .txt" << endl;
            }
        } else {
            cout << "Uso incorrecto. Sintaxis: inicializar <nombre_archivo.txt>" << endl;
        }
    }
    else if (comando_principal == "Atacar" || comando_principal == "atacar") {
        if (cantidad_palabras == 2) {
            // LLAMADO REAL A JUEGO: Le pasa el nombre del jugador y deja el territorio vacio para pedirlo dentro del método
            juego.AtacarTerritorio(palabras_comando[1], "");
        } else {
            cout << "Uso incorrecto. Sintaxis: atacar <nombre_jugador>" << endl;
        }
    }
    else if (comando_principal == "Obtener_unidades" || comando_principal == "obtener_unidades") {
        if (cantidad_palabras == 2) {
              if (esNombreJugadorValido(palabras_comando[1])) {
                juego.ReclamarUnidades(palabras_comando[1]);
            } else {
                cout << "(Jugador no valido) El nombre debe tener maximo 8 caracteres." << endl;
            }
        } else {
            cout << "Uso incorrecto. Sintaxis: obtener_unidades <nombre_jugador>" << endl;
        }
    }
    else if (comando_principal == "Fortificar" || comando_principal == "fortificar") {
        if (cantidad_palabras == 2) {
            if (esNombreJugadorValido(palabras_comando[1])) {
                juego.FortificarTerritorio(palabras_comando[1], "");
            } else {
                cout << "(Jugador no valido) El nombre debe tener maximo 8 caracteres." << endl;
            }
        } else {
            cout << "Uso incorrecto. Sintaxis: fortificar <nombre_jugador>" << endl;
        }
    }
    else if (comando_principal == "Estado_juego" || comando_principal == "estado_juego") {
        if (cantidad_palabras == 1) {
            juego.EstadoJuego();
            
        } else {
            cout << "Uso incorrecto. Sintaxis: estado_juego (no recibe parametros)" << endl;
        }
    }
    else if (comando_principal == "Guardar" || comando_principal == "guardar") {
        if (cantidad_palabras == 2) {
            // TODO: IMPLEMENTAR METODO REAL -> juego.GuardarPartida(palabras_comando[1]);
            cout << "(Pendiente implementar) Comando guardar en: " << palabras_comando[1] << endl;
        } else {
            cout << "Uso incorrecto. Sintaxis: guardar <nombre_archivo>" << endl;
        }
    }
    else if (comando_principal == "Guardar_comprimido" || comando_principal == "guardar_comprimido") {
        if (cantidad_palabras == 2) {
            // TODO: IMPLEMENTAR METODO REAL -> juego.GuardarComprimido(palabras_comando[1]);
            cout << "(Pendiente implementar) Comando guardar_comprimido en: " << palabras_comando[1] << endl;
        } else {
            cout << "Uso incorrecto. Sintaxis: guardar_comprimido <nombre_archivo>" << endl;
        }
    }
    else if (comando_principal == "Costo_conquista" || comando_principal == "costo_conquista") {
        if (cantidad_palabras == 3) {
            if (esNombreJugadorValido(palabras_comando[1])) {
                // TODO: IMPLEMENTAR METODO REAL (Grafo/Dijkstra) -> juego.CostoConquista(palabras_comando[1], palabras_comando[2]);
                cout << "(Pendiente implementar) Comando costo_conquista para: " << palabras_comando[1] << " hacia " << palabras_comando[2] << endl;
            } else {
                cout << "(Jugador no valido) El nombre debe tener maximo 8 caracteres." << endl;
            }
        } else {
            cout << "Uso incorrecto. Sintaxis: costo_conquista <nombre_jugador> <territorio>" << endl;
        }
    }
    else if (comando_principal == "Conquista_mas_barata" || comando_principal == "conquista_mas_barata") {
        if (cantidad_palabras == 2) {
            if (esNombreJugadorValido(palabras_comando[1])) {
                // TODO: IMPLEMENTAR METODO REAL -> juego.ConquistaMasBarata(palabras_comando[1]);
                cout << "(Pendiente implementar) Comando conquista_mas_barata para: " << palabras_comando[1] << endl;
            } else {
                cout << "(Jugador no valido) El nombre debe tener maximo 8 caracteres." << endl;
            }
        } else {
            cout << "Uso incorrecto. Sintaxis: conquista_mas_barata <nombre_jugador>" << endl;
        }
    }
    else if (comando_principal == "Salir" || comando_principal == "salir") {
        if (cantidad_palabras == 1) {
            ejecutando = false;
        } else {
             cout << "Uso incorrecto. Sintaxis: salir (no recibe parametros)" << endl;
        }
    } 
    else {
        cout << "Comando no reconocido. Escribe 'ayuda' para ver la lista de comandos." << endl;
    }
}

void iniciarConsola() {
    Juego juego;
    string entrada;
    bool ejecutando = true;

    cout << R"(
 ____  ___ ____  _  __    ____ _     ___ _____ _   _ _____ 
|  _ \|_ _/ ___|| |/ /    / ___| |   |_ _| ____| \ | |_   _|
| |_) || |\___ \| ' /    | |   | |    | ||  _| |  \| | | |  
|  _ < | | ___) | . \    | |___| |___ | || |___| |\  | | |  
|_| \_\___|____/|_|\_\    \____|_____|___|_____|_| \_| |_|  
)" << endl;
    cout << "=================================================================" << endl;
    cout << "         Sistema de Control y Estrategia - Risk C++              " << endl;
    cout << "=================================================================" << endl;
    cout << "  Escribe 'ayuda' para desplegar la lista de comandos." << endl;
    cout << "  Escribe 'salir' para finalizar la partida." << endl;
    cout << "=================================================================\n" << endl;

    while (ejecutando) {
        cout << "$ ";
        if (!getline(cin, entrada)) break;

        vector<string> palabras = separarPalabras(entrada);
        procesarEntrada(palabras, ejecutando, juego);
    }
    cout << endl;
    cout << "=================================================================" << endl;
    cout << "      ¡Gracias por jugar! Has salido con exito de Risk           " << endl;
    cout << "=================================================================" << endl;
}
