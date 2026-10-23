#include "Juego.h"
#include "Jugador.h"
#include "Territorio.h"
#include "Continente.hpp"
#include "ArbolHuffman.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <map>

using namespace std;

namespace {
    void ImprimirSeparadorEstado(int anchoNumero, int anchoNombre, int anchoColor, int anchoTurno) {
        cout << "+" << string(anchoNumero + 2, '-')
             << "+" << string(anchoNombre + 2, '-')
             << "+" << string(anchoColor + 2, '-')
             << "+" << string(anchoTurno + 2, '-') << "+" << endl;
    }

    bool LeerEnteroConsola(const string& mensaje, int& valor) {
        while (true) {
            cout << mensaje;
            string entrada;
            if (!getline(cin, entrada)) {
                return false;
            }

            stringstream flujo(entrada);
            char caracterExtra;
            if (flujo >> valor && !(flujo >> caracterExtra)) {
                return true;
            }

            cout << "(Entrada inválida) Ingrese un número entero." << endl;
        }
    }

    bool LeerEnteroBinario(ifstream& archivo, int cantidadBytes, long long& valor) {
        valor = 0;
        for (int byte = 0; byte < cantidadBytes; ++byte) {
            char dato;
            if (!archivo.get(dato)) {
                return false;
            }
            int valorByte = dato;
            if (valorByte < 0) {
                valorByte += 256;
            }
            if (byte == 7 && valorByte > 127) {
                return false;
            }
            // El archivo guarda los enteros en orden little-endian.
            valor |= static_cast<long long>(valorByte) << (8 * byte);
        }
        return true;
    }
}

bool Juego::DecodificarArchivoComprimido(const string& nombreArchivo, string& texto) const {
    texto.clear();
    ifstream archivo(nombreArchivo.c_str(), ios::binary);
    if (!archivo.is_open()) {
        return false;
    }
    // El encabezado indica los símbolos y frecuencias usados para crear el árbol.
    long long cantidadSimbolos;
    if (!LeerEnteroBinario(archivo, 2, cantidadSimbolos) ||
        cantidadSimbolos <= 0 || cantidadSimbolos > 256) {
        return false;
    }

    map<char, long long> frecuencias;
    for (int i = 0; i < cantidadSimbolos; ++i) {
        char simbolo;
        long long frecuencia;
        if (!archivo.get(simbolo) ||
            !LeerEnteroBinario(archivo, 8, frecuencia) ||
            frecuencia <= 0 ||
            frecuencias.find(simbolo) != frecuencias.end()) {
            return false;
        }

        frecuencias[simbolo] = frecuencia;
    }

    long long longitudTexto;
    if (!LeerEnteroBinario(archivo, 8, longitudTexto) ||
        longitudTexto <= 0) {
        return false;
    }

    // Las frecuencias permiten reconstruir el árbol usado al comprimir.
    ArbolHuffman arbol(frecuencias);
    Nodo* raiz = arbol.ObtenerRaiz();
    if (raiz == 0) {
        return false;
    }

    string textoDecodificado;
    long long caracteresDecodificados = 0;
    Nodo* nodo = raiz;
    int paquete = 0;
    int bitsRestantes = 0;

    // Se lee hasta recuperar la longitud original; los bits finales pueden ser relleno.
    while (caracteresDecodificados < longitudTexto) {
        if (bitsRestantes == 0) {
            char dato;
            if (!archivo.get(dato)) {
                return false;
            }
            paquete = dato;
            if (paquete < 0) {
                paquete += 256;
            }
            bitsRestantes = 8;
        }

        int bit = (paquete >> (bitsRestantes - 1)) & 1;
        --bitsRestantes;

        if (raiz->EsHoja()) {
            if (bit != 0) {
                return false;
            }
            nodo = raiz;
        } else {
            nodo = bit == 0 ? nodo->ObtenerIzquierdo()
                            : nodo->ObtenerDerecho();
            if (nodo == 0) {
                return false;
            }
        }

        if (nodo->EsHoja()) {
            char simbolo = nodo->ObtenerSimbolo();
            textoDecodificado += simbolo;
            ++caracteresDecodificados;
            // Cada hoja completa un símbolo; el siguiente código empieza en la raíz.
            nodo = raiz;
        }
    }

    // Solo se publica el resultado cuando toda la validación termina correctamente.
    texto.swap(textoDecodificado);
    return true;
}

Juego::Juego()
{

    juegoInicializado = false; // Conecta con el TAD asi sabemos que el juego no se ha iniciado
    jugadorActual = nullptr;
    juegoTerminado = false;
}

// Estructura lineal auxiliar exclusivamente para definir las fronteras del tablero
struct ConexionFrontera
{
    string territorio1;
    string territorio2;
};

// validar existencia de jugador
Jugador *Juego::BuscarJugador(const string &nombre)
{
    for (Jugador *j : jugadores)
    {
        if (j->ObtenerNombre() == nombre)
        {
            return j;
        }
    }
    return nullptr;
}
// validar existencia de territorio
Territorio *Juego::BuscarTerritorio(const string &busqueda)
{
    for (Continente &continente : continentes)
    {
        for (Territorio *territorio : continente.ObtenerTerritorios())
        {
            if (territorio->ObtenerCodigo() == busqueda || territorio->ObtenerNombre() == busqueda)
            {
                return territorio;
            }
        }
    }
    return nullptr;
}
Continente *Juego::BuscarContinente(const string &nombre)
{
    for (Continente &continente : continentes)
    {
        if (continente.ObtenerNombre() == nombre)
        {
            return &continente;
        }
    }
    return nullptr;
}
void Juego::ConfigurarFronteras()
{ // Los jugadores no se agregan dos veces porque el metodo agregar vecino revisa que no existe otro antes
   vector<ConexionFrontera> fronteras_del_mundo = {
    // América del Norte
    {"1.1", "1.2"}, {"1.1", "1.6"}, {"1.1", "5.6"},
    {"1.2", "1.6"}, {"1.2", "1.7"}, {"1.2", "1.9"},
    {"1.3", "1.4"}, {"1.3", "1.9"}, {"1.3", "2.4"},
    {"1.4", "1.7"}, {"1.4", "1.8"}, {"1.4", "1.9"},
    {"1.5", "1.6"}, {"1.5", "1.7"}, {"1.5", "1.8"}, {"1.5", "3.2"},
    {"1.6", "1.7"},
    {"1.7", "1.8"}, {"1.7", "1.9"},

    // América del Sur
    {"2.1", "2.2"}, {"2.1", "2.3"},
    {"2.2", "2.3"}, {"2.2", "2.4"}, {"2.2", "4.5"},
    {"2.3", "2.4"},

    // Europa
    {"3.1", "3.2"}, {"3.1", "3.3"}, {"3.1", "3.4"}, {"3.1", "3.7"},
    {"3.2", "3.4"},
    {"3.3", "3.4"}, {"3.3", "3.5"}, {"3.3", "3.6"}, {"3.3", "3.7"},
    {"3.4", "3.6"},
    {"3.5", "3.6"}, {"3.5", "3.7"}, {"3.5", "4.3"}, {"3.5", "4.5"}, {"3.5", "5.7"},
    {"3.6", "5.1"}, {"3.6", "5.7"}, {"3.6", "5.11"},
    {"3.7", "4.5"},

    // África
    {"4.1", "4.2"}, {"4.1", "4.5"}, {"4.1", "4.6"},
    {"4.2", "4.3"}, {"4.2", "4.4"}, {"4.2", "4.5"}, {"4.2", "4.6"}, {"4.2", "5.7"},
    {"4.3", "4.5"}, {"4.3", "5.7"},
    {"4.4", "4.6"},

    // Asia
    {"5.1", "5.2"}, {"5.1", "5.3"}, {"5.1", "5.7"}, {"5.1", "5.11"},
    {"5.2", "5.3"}, {"5.2", "5.8"}, {"5.2", "5.9"},
    {"5.3", "5.7"}, {"5.3", "5.9"},
    {"5.4", "5.6"}, {"5.4", "5.8"},
    {"5.5", "5.6"}, {"5.5", "5.8"},
    {"5.6", "5.8"},
    {"5.9", "6.2"},
    {"5.10", "5.2"}, {"5.10", "5.4"}, {"5.10", "5.8"}, {"5.10", "5.11"}, {"5.10", "5.12"},
    {"5.11", "5.2"},
    {"5.12", "5.4"}, {"5.12", "5.6"},

    // Australia
    {"6.1", "6.3"}, {"6.1", "6.4"},
    {"6.2", "6.3"}, {"6.2", "6.4"}
};

    // Recorrido estrictamente lineal para establecer la relación bidireccional
    for (size_t i = 0; i < fronteras_del_mundo.size(); i++)
    {
        Territorio *t1 = BuscarTerritorio(fronteras_del_mundo[i].territorio1);
        Territorio *t2 = BuscarTerritorio(fronteras_del_mundo[i].territorio2);

        if (t1 != nullptr && t2 != nullptr)
        {
            t1->AgregarVecino(t2);
            t2->AgregarVecino(t1); // Relación bidireccional automática
            // La frontera entre territorios también conecta sus continentes.
            Continente *continente1 = BuscarContinente(t1->ObtenerContinente());
            Continente *continente2 = BuscarContinente(t2->ObtenerContinente());
            if (continente1 != nullptr && continente2 != nullptr && continente1 != continente2)
            {
                continente1->AgregarVecino(continente2);
                continente2->AgregarVecino(continente1);
            }
        }
    }
}

void Juego::InicializarJuego(const string &archivo)
{
    if (juegoInicializado)
    {
        cout << "(Juego en curso) El juego ya ha sido inicializado." << endl;
        return;
    }

    ifstream archivoEntrada(archivo.c_str());
    if (!archivoEntrada.is_open())
    {
        cerr << "Error: No se pudo abrir el archivo: " << archivo << endl;
        return;
    }

    ostringstream contenido;
    contenido << archivoEntrada.rdbuf();
    InicializarDesdeTexto(archivo, contenido.str(), true);
}

void Juego::inicializar(string archivo_inicio)
{
    if (juegoInicializado)
    {
        cout << "(Juego en curso) El juego ya ha sido inicializado." << endl;
        return;
    }

    string texto;
    if (!DecodificarArchivoComprimido(archivo_inicio, texto))
    {
        cerr << "(Archivo comprimido invalido) No se pudo decodificar "
             << archivo_inicio << "." << endl;
        return;
    }

    InicializarDesdeTexto(archivo_inicio, texto, false);
}

void Juego::InicializarDesdeTexto(const string& archivo, const string& texto,
                                  bool validarUnidadesIniciales)
{
    //agregado para diferenciar si es un archivo de partida guardada o iniciar una partida nueva
    const string marcadorPartidaGuardada = "# Bandera_Archivo_Guardado:v";
    string textoEntrada = texto;
    if (textoEntrada.compare(0, marcadorPartidaGuardada.size(),
                             marcadorPartidaGuardada) == 0) {
        size_t finMarcador = textoEntrada.find('\n');
        if (finMarcador == string::npos) {
            textoEntrada.clear();
        } else {
            textoEntrada.erase(0, finMarcador + 1);
        }
        validarUnidadesIniciales = false;
    }

    vector<DatosTerritorio> baseTerritorios = {// Este vector nos va a permitir encontrar los datos a partir del codigo del archivo, funciona como una base
                                               {"1.1", "Alaska", "America del Norte"},
                                               {"1.2", "Alberta", "America del Norte"},
                                               {"1.3", "America Central", "America del Norte"},
                                               {"1.4", "Estados Unidos Orientales", "America del Norte"},
                                               {"1.5", "Groenlandia", "America del Norte"},
                                               {"1.6", "Territorio Noroccidental", "America del Norte"},
                                               {"1.7", "Ontario", "America del Norte"},
                                               {"1.8", "Quebec", "America del Norte"},
                                               {"1.9", "Estados Unidos Occidentales", "America del Norte"},

                                               {"2.1", "Argentina", "America del Sur"},
                                               {"2.2", "Brasil", "America del Sur"},
                                               {"2.3", "Peru", "America del Sur"},
                                               {"2.4", "Venezuela", "America del Sur"},

                                               {"3.1", "Gran Bretana", "Europa"},
                                               {"3.2", "Islandia", "Europa"},
                                               {"3.3", "Europa del Norte", "Europa"},
                                               {"3.4", "Escandinavia", "Europa"},
                                               {"3.5", "Europa del Sur", "Europa"},
                                               {"3.6", "Ucrania", "Europa"},
                                               {"3.7", "Europa Occidental", "Europa"},

                                               {"4.1", "Congo", "Africa"},
                                               {"4.2", "Africa Oriental", "Africa"},
                                               {"4.3", "Egipto", "Africa"},
                                               {"4.4", "Madagascar", "Africa"},
                                               {"4.5", "Africa del Norte", "Africa"},
                                               {"4.6", "Africa del Sur", "Africa"},

                                               {"5.1", "Afghanistan", "Asia"},
                                               {"5.2", "China", "Asia"},
                                               {"5.3", "India", "Asia"},
                                               {"5.4", "Irkutsk", "Asia"},
                                               {"5.5", "Japon", "Asia"},
                                               {"5.6", "Kamchatka", "Asia"},
                                               {"5.7", "Medio Oriente", "Asia"},
                                               {"5.8", "Mongolia", "Asia"},
                                               {"5.9", "Siam", "Asia"},
                                               {"5.10", "Siberia", "Asia"},
                                               {"5.11", "Ural", "Asia"},
                                               {"5.12", "Yakutsk", "Asia"},

                                               {"6.1", "Australia Oriental", "Australia"},
                                               {"6.2", "Indonesia", "Australia"},
                                               {"6.3", "Nueva Guinea", "Australia"},
                                               {"6.4", "Australia Occidental", "Australia"}};

    // Crear continentes a partir de los nombres ya definidos en baseTerritorios.
    for (const DatosTerritorio &dato : baseTerritorios)
    {
        if (BuscarContinente(dato.continente) == nullptr)
        {
            continentes.push_back(Continente(dato.continente));
        }
    }
    istringstream archivoEntrada(textoEntrada);

    if (archivoEntrada.peek() == ifstream::traits_type::eof())
    { // Revisamos que si tenga info el archivo
        cout << "(Archivo vacío) " << archivo << " no contiene información." << endl;
        return;
    }

    int cantidadJugadores;

    if (!(archivoEntrada >> cantidadJugadores))
    {
        cout << "(Archivo sin formato) " << archivo << " no contiene información en el formato esperado." << endl;
        return;
    }

    int unidadesPermitidas = 0; // Nos va a servir para cumplir la condicion de saber cuantas unidades puede tener un jugador

    if (cantidadJugadores == 3)
    {
        unidadesPermitidas = 35;
    }
    else if (cantidadJugadores == 4)
    {
        unidadesPermitidas = 30;
    }
    else if (cantidadJugadores == 5)
    {
        unidadesPermitidas = 25;
    }
    else if (cantidadJugadores == 6)
    {
        unidadesPermitidas = 20;
    }
    else
    {
        cout << "(Cantidad de jugadores inválida) El juego solo puede ser jugado por 3, 4, 5 o 6 jugadores." << endl;
        return;
    }

    for (int i = 0; i < cantidadJugadores; i++)
    { // Crear jugadores

        string nombreJugador;
        string colorJugador;

        if (!(archivoEntrada >> nombreJugador >> colorJugador))
        { // Condicion para saber si no hay nada de info
            cout << "(Archivo sin formato) " << archivo << " no contiene información en el formato esperado." << endl;
            return;
        }

        Jugador *jugador = new Jugador(nombreJugador, colorJugador);
        jugadores.push_back(jugador); // Se agrega el jugador a la lista de los jugadores de juego
    }

    // Leer territorios y asociarlos a partir de su codigo

    string codigoTerritorio;
    string color;
    int unidades;
    int conteoTerritorios = 0;

    while (archivoEntrada >> codigoTerritorio >> color >> unidades)
    { // Va a leer toda la info

        conteoTerritorios++;      // Va a contar la cantidad de territorios que hay en el archivo
        Jugador *Dueno = nullptr; // Hay que declararlo dentro para que no se quede con la direccion del jugador anterior

        for (Jugador *jugador : jugadores)
        {
            if (jugador->ObtenerColor() == color)
            { // Va a asociar el color del jugador con el color de ese territorio para saber quien es el Dueno

                Dueno
                 = jugador;
                Dueno
                ->AgregarEjercito(unidades); // Revisamos que si hubiese un control
                break;
            }
        }

        if (Dueno
             == nullptr)
        {

            cout << "(Archivo sin formato) " << archivo << " no contiene información en el formato esperado." << endl;
            return;
        }

        string nombreContinente; // Comparacion para asignar el continente al territorio segun su codigo
        string nombreTerritorio;
        bool bandera = false;

        for (vector<DatosTerritorio>::iterator it = baseTerritorios.begin(); bandera == false && it != baseTerritorios.end(); it++)
        {
            // A partir de la base que tenemos podemos identificar los territorios y se sale de forma segura POR SI EN ALGUN CASO no se encuentra

            if (it->codigo == codigoTerritorio)
            {

                nombreContinente = it->continente;
                nombreTerritorio = it->nombre;
                bandera = true;
            }
        }

        Territorio *territorio = new Territorio(nombreTerritorio, codigoTerritorio, nombreContinente, Dueno
            , unidades);
        territorios.push_back(territorio);
        Continente *continente = BuscarContinente(nombreContinente);
        if (continente != nullptr)
        {
            continente->AgregarTerritorio(territorio);
        } // Se agrega el territorio a la lista de territorios de juego
        if (Dueno
             != nullptr)
        {
            Dueno
            ->AgregarTerritorio(territorio); // Se agrega el territorio a la lista de territorios del jugador Dueno

        }
    }

    if (conteoTerritorios != 42)
    { // Revisamos que sean solo los 42 territorios
        cout << "(Cantidad de territorios inválida) El juego debe contener exactamente 42 territorios." << endl;
        return;
    }

    for (Jugador *jugador : jugadores)
    {

        if (validarUnidadesIniciales &&
            jugador->ObtenerEjercito() != unidadesPermitidas)
        {

            cout << "(Cantidad de unidades invalida) El jugador " << jugador->ObtenerNombre() << " tiene una cantidad de unidades diferente a la permitida." << endl;
            return;
        }
    }

    ConfigurarFronteras();              // Se llama a la funcion para configurar las fronteras del juego
    juegoInicializado = true;  
    juegoTerminado = false;         // Si todo esta bien, el juego se inicializa
    jugadorActual = jugadores.front();  // El primer jugador de la lista va a ser el que tenga el turno inicial
    srand(time(0));                     // Inicializa la semilla para la generación de números aleatorios basada en el tiempo actual
    cout << "(Juego inicializado) El juego ha sido inicializado correctamente." << endl;
}

void Juego::EliminarJugadorDeJuego(Jugador *jugadorAEliminar)
{
    if (jugadorAEliminar == nullptr)
        return;

    for (size_t iteliminar = 0; iteliminar < jugadores.size(); iteliminar++) //Cambio en el tipo de variable para usar size
    {
        if (jugadores[iteliminar] == jugadorAEliminar)
        {
            jugadores.erase(jugadores.begin() + iteliminar);
            delete jugadorAEliminar;
            break;
        }
    }
}

void Juego::CambiarTurno(const string& jugador) {
    if (jugadores.empty()) return;

    for (size_t i = 0; i < jugadores.size(); i++) {
        if (jugadores[i]->ObtenerNombre() == jugador) {
            size_t siguiente = (i + 1) % jugadores.size();
            jugadorActual = jugadores[siguiente];
            //Reiniciar cuando se empieza otro turno

            jugadorActual->ReiniciarEstadoTurno();
            return;
        }
    }

}

bool Juego ::VerificarGanador(){
    for (Jugador* jugador : jugadores){
        if(jugador->ObtenerTerritorios().size() == 42){
            return true;
        }
    }
    return false; 
}

void Juego::ReclamarUnidades(const string& nombreJugador){
    if(!juegoInicializado){
        cout<<"(Juego no inicializado) Esta partida no ha sido inicializada correctamente." << endl;
        return;
    }

    if (juegoTerminado){
        cout << "Este juego ya ha finalizado. " << endl;
        return;
    }

    Jugador* jg = BuscarJugador(nombreJugador);
    if (jg == nullptr) {
        cout << "(Jugador no valido) El jugador " << nombreJugador << " no forma parte de esta partida." << endl;
        return;
    }

    if(jg !=jugadorActual){
        cout << "(Jugador fuera de turno) No es el turno del jugador " << nombreJugador << "." << endl;
        return;
    }

    if (jg->ObtenerUnidadesReclamadas()) {
    cout << "(Unidades ya reclamadas) El jugador " << nombreJugador << " ya ha reclamado sus unidades en este turno." << endl;
    return;
    }

    //Cuando obtienen nuevas unidades de ejercito por Territorios
    int cantTerritorios = jg->ObtenerTerritorios().size();
    int unidadesReclamar = cantTerritorios/3;

    // Los continentes determinan el bono adicional por control completo.
    for (const Continente &continente : continentes) {
        if (continente.EstaControladoPor(jg)) {
            unidadesReclamar += continente.ObtenerBonificacion();
        }
    }
    cout << "El jugador " << nombreJugador << " puede reclamar " << unidadesReclamar << " unidades nuevas." << endl;

    // Ubicar las unidades nuevas
    int unidadesRestantes = unidadesReclamar;
    while (unidadesRestantes > 0) {
        string codigoTerritorio;
        int cantidad;

        cout << "Unidades restantes por asignar: " << unidadesRestantes << endl;
        cout << "Ingrese el codigo del territorio: ";
        if (!getline(cin, codigoTerritorio)) {
            return;
        }
        if (!LeerEnteroConsola("Ingrese la cantidad de unidades a asignar: ", cantidad)) {
            return;
        }

        Territorio* territorio = BuscarTerritorio(codigoTerritorio);

        if (territorio == nullptr || territorio->ObtenerDueno
            () != jg) {
            cout << "Territorio invalido o no pertenece al jugador." << endl;
            continue;
        }

        if (cantidad <= 0 || cantidad > unidadesRestantes) {
            cout << "Cantidad invalida." << endl;
            continue;
        }

        territorio->AgregarUnidades(cantidad);
        jg->AgregarEjercito(cantidad);
        unidadesRestantes -= cantidad;
    }

    jg->EstablecerUnidadesReclamadas(true);
    cout << "(Comando correcto) El jugador " << nombreJugador << " ha terminado de reclamar y ubicar sus unidades." << endl;  
}

void Juego::EstadoJuego(){
    if (!juegoInicializado) {
        cout << "(Juego no inicializado) Esta partida no ha sido inicializada correctamente." << endl;
        return;
    }

    if (juegoTerminado)
{
    cout << "(Juego terminado) Esta partida ya tuvo un ganador." << endl;
    return;
}

int anchoNombreJugador = static_cast<int>(string("Nombre").size());
int anchoColorJugador = static_cast<int>(string("Color").size());
for (Jugador* jugador : jugadores) {
    anchoNombreJugador = max(anchoNombreJugador,
                             static_cast<int>(jugador->ObtenerNombre().size()));
    anchoColorJugador = max(anchoColorJugador,
                            static_cast<int>(jugador->ObtenerColor().size()));
}

const int anchoNumero = 3;
const int anchoTurno = 7;
const int anchoTablaJugadores = anchoNumero + anchoNombreJugador
                              + anchoColorJugador + anchoTurno + 10;

cout << "\n" << string(anchoTablaJugadores, '=') << endl;
cout << "ESTADO DEL JUEGO" << endl;
cout << string(anchoTablaJugadores, '=') << endl;
cout << "Jugadores: " << jugadores.size() << endl;
ImprimirSeparadorEstado(anchoNumero, anchoNombreJugador, anchoColorJugador, anchoTurno);
cout << "| " << left << setw(anchoNumero) << "No." << " "
     << "| " << setw(anchoNombreJugador) << "Nombre" << " "
     << "| " << setw(anchoColorJugador) << "Color" << " "
     << "| " << setw(anchoTurno) << "Turno" << " |" << endl;
ImprimirSeparadorEstado(anchoNumero, anchoNombreJugador, anchoColorJugador, anchoTurno);

for (size_t i = 0; i < jugadores.size(); ++i) {
    Jugador* jugador = jugadores[i];
    string estadoTurno = "";
    if (jugador == jugadorActual) {
        estadoTurno = "ACTUAL";
    }
    cout << "| " << left << setw(anchoNumero) << i + 1 << " "
         << "| " << setw(anchoNombreJugador) << jugador->ObtenerNombre() << " "
         << "| " << setw(anchoColorJugador) << jugador->ObtenerColor() << " "
         << "| " << setw(anchoTurno) << estadoTurno << " |" << endl;
}
ImprimirSeparadorEstado(anchoNumero, anchoNombreJugador, anchoColorJugador, anchoTurno);

cout << "Turno actual: ";
if (jugadorActual != nullptr) {
    cout << jugadorActual->ObtenerNombre() << endl;
} else {
    cout << "ninguno" << endl;
}

int anchoCodigo = static_cast<int>(string("Codigo").size());
int anchoTerritorio = static_cast<int>(string("Territorio").size());
int anchoDueno = static_cast<int>(string("Dueno").size());
int anchoColor = static_cast<int>(string("Color").size());
int anchoUnidades = static_cast<int>(string("Unidades").size());
for (Territorio* territorio : territorios) {
    Jugador* dueno = territorio->ObtenerDueno();
    anchoCodigo = max(anchoCodigo, static_cast<int>(territorio->ObtenerCodigo().size()));
    anchoTerritorio = max(anchoTerritorio, static_cast<int>(territorio->ObtenerNombre().size()));
    if (dueno != nullptr) {
        anchoDueno = max(anchoDueno, static_cast<int>(dueno->ObtenerNombre().size()));
        anchoColor = max(anchoColor, static_cast<int>(dueno->ObtenerColor().size()));
    } else {
        anchoDueno = max(anchoDueno, static_cast<int>(string("Sin dueño").size()));
        anchoColor = max(anchoColor, static_cast<int>(string("-").size()));
    }
    ostringstream flujoUnidades;
    flujoUnidades << territorio->ObtenerUnidades();
    string textoUnidades = flujoUnidades.str();
    anchoUnidades = max(anchoUnidades, static_cast<int>(textoUnidades.size()));
}

const int anchoTablaTerritorios = anchoCodigo + anchoTerritorio + anchoDueno
                                + anchoColor + anchoUnidades + 12;
cout << "\n" << string(anchoTablaTerritorios, '=') << endl;
cout << "TERRITORIOS" << endl;
cout << string(anchoTablaTerritorios, '=') << endl;
cout << "| " << left << setw(anchoCodigo) << "Codigo" << " "
     << "| " << setw(anchoTerritorio) << "Territorio" << " "
     << "| " << setw(anchoDueno) << "Dueno" << " "
     << "| " << setw(anchoColor) << "Color" << " "
     << "| " << right << setw(anchoUnidades) << "Unidades" << " |" << endl;
cout << string(anchoTablaTerritorios, '-') << endl;

for (Territorio* territorio : territorios) {
    Jugador* dueno = territorio->ObtenerDueno();
    string nombreDueno = "Sin dueño";
    string colorDueno = "-";
    if (dueno != nullptr) {
        nombreDueno = dueno->ObtenerNombre();
        colorDueno = dueno->ObtenerColor();
    }
    cout << "| " << left << setw(anchoCodigo) << territorio->ObtenerCodigo() << " "
         << "| " << setw(anchoTerritorio) << territorio->ObtenerNombre() << " "
         << "| " << setw(anchoDueno) << nombreDueno << " "
         << "| " << setw(anchoColor) << colorDueno << " "
         << "| " << right << setw(anchoUnidades) << territorio->ObtenerUnidades() << " |" << endl;
}
cout << string(anchoTablaTerritorios, '=') << endl;
}

string Juego::GenerarTextoPartida() const {
    ostringstream texto;
    texto << "# Bandera_Archivo_Guardado:v\n";
    texto << jugadores.size() << '\n';

    size_t posicionActual = 0;
    for (size_t i = 0; i < jugadores.size(); ++i) {
        if (jugadores[i] == jugadorActual) {
            posicionActual = i;
            break;
        }
    }

    if (!jugadores.empty()) {
        for (size_t k = 0; k < jugadores.size(); ++k) {
            Jugador* jugador = jugadores[(posicionActual + k) % jugadores.size()];
            texto << jugador->ObtenerNombre() << ' '
                  << jugador->ObtenerColor() << '\n';
        }
    }

    for (Territorio* territorio : territorios) {
        Jugador* dueno = territorio->ObtenerDueno();
        const string color = dueno != nullptr ? dueno->ObtenerColor() : "sin_dueño";
        texto << territorio->ObtenerCodigo() << ' '
              << color << ' '
              << territorio->ObtenerUnidades() << '\n';
    }

    return texto.str();
}

void Juego::GuardarPartida(const string& nombreArchivo) {
    if (!juegoInicializado) {
        cout << "(Juego no inicializado) Esta partida no ha sido inicializada correctamente." << endl;
        return;
    }

    string nombre = nombreArchivo;
    if (nombre.size() < 4 || nombre.substr(nombre.size() - 4) != ".txt") {
        nombre += ".txt";
    }

    ofstream archivoSalida(nombre.c_str());
    if (!archivoSalida.is_open()) {
        cout << "(No se puede guardar) No se pudo crear el archivo: "
             << nombre << endl;
        return;
    }

    archivoSalida << GenerarTextoPartida();
    if (!archivoSalida.good()) {
        cout << "(No se puede guardar) Ocurrio un error al escribir en el archivo: "
             << nombre << endl;
        return;
    }

    archivoSalida.close();
    if (!archivoSalida) {
        cout << "(No se puede guardar) Ocurrio un error al cerrar el archivo: "
             << nombre << endl;
        return;
    }

    cout << "(Comando correcto) La partida ha sido guardada en "
         << nombre << "." << endl;
}

void Juego::GuardarComprimido(const string& nombreArchivo) {
    if (!juegoInicializado) {
        cout << "(Juego no inicializado) No se puede guardar un estado inexistente." << endl;
        return;
    }

    string nombre = nombreArchivo;
    if (nombre.size() < 4 || nombre.substr(nombre.size() - 4) != ".bin") {
        nombre += ".bin";
    }

    for (Territorio* territorio : territorios) {
        if (territorio->ObtenerDueno() == 0) {
            cout << "(Estado invalido) Hay un territorio sin jugador propietario." << endl;
            return;
        }
    }

    string texto = GenerarTextoPartida();
    map<char, long long> frecuencias;
    for (char caracter : texto) {
        frecuencias[caracter]++;
    }

    ArbolHuffman arbol(frecuencias);
    const map<char, string>& diccionario = arbol.ObtenerDiccionario();

    ofstream archivoSalida(nombre.c_str(), ios::binary);
    if (!archivoSalida.is_open()) {
        cerr << "Error: No se pudo crear el archivo: " << nombre << endl;
        return;
    }

    // Se escriben los enteros byte a byte en orden little-endian.
    int cantidadSimbolos = static_cast<int>(frecuencias.size());
    for (int byte = 0; byte < 2; ++byte) {
        archivoSalida.put(static_cast<char>((cantidadSimbolos >> (8 * byte)) & 255));
    }

    map<char, long long>::const_iterator frecuencia;
    for (frecuencia = frecuencias.begin(); frecuencia != frecuencias.end(); ++frecuencia) {
        archivoSalida.put(frecuencia->first);
        for (int byte = 0; byte < 8; ++byte) {
            archivoSalida.put(static_cast<char>((frecuencia->second >> (8 * byte)) & 255));
        }
    }

    long long longitudTexto = static_cast<long long>(texto.size());
    for (int byte = 0; byte < 8; ++byte) {
        archivoSalida.put(static_cast<char>((longitudTexto >> (8 * byte)) & 255));
    }

    int paquete = 0;
    int bitsEnPaquete = 0;
    for (char caracter : texto) {
        map<char, string>::const_iterator codigo = diccionario.find(caracter);
        if (codigo == diccionario.end()) {
            cerr << "Error: No se encontro el codigo Huffman de un simbolo." << endl;
            return;
        }

        for (int j = 0; j < static_cast<int>(codigo->second.size()); ++j) {
            paquete = paquete << 1;
            if (codigo->second[j] == '1') {
                paquete = paquete | 1;
            }
            ++bitsEnPaquete;
            if (bitsEnPaquete == 8) {
                archivoSalida.put(static_cast<char>(paquete));
                paquete = 0;
                bitsEnPaquete = 0;
            }
        }
    }

    if (bitsEnPaquete > 0) {
        paquete = paquete << (8 - bitsEnPaquete);
        archivoSalida.put(static_cast<char>(paquete));
    }

    if (!archivoSalida) {
        cerr << "Error: Ocurrio un problema al escribir el archivo comprimido." << endl;
        return;
    }

    cout << "(Guardado comprimido) Estado guardado en " << nombre << endl;
}

void Juego::guardar_comprimido(string nombre_archivo) {
    GuardarComprimido(nombre_archivo);
}
void Juego::FortificarTerritorio(const string& jugador, const string& territorio) {
    
    // Verificar si el juego está inicializado
    if (!juegoInicializado) {
        cout << "Esta partida no ha sido inicializada correctamente." << endl;
        return;
    }

    if (juegoTerminado){
        cout << "Este juego ya ha finalizado. " << endl;
        return;
    }
    
    // Verificar si hay un ganador
    if (VerificarGanador()) {
        cout << "Esta partida ya tuvo un ganador." << endl;
        return;
    }
    
    //  Buscar el jugador
    Jugador* jugadorActualObj = BuscarJugador(jugador);
    if (jugadorActualObj == nullptr) {
        cout << "El jugador " << jugador << " no forma parte de esta partida." << endl;
        return;
    }

    if (!jugadorActualObj->ObtenerHaAtacado()) {
    cout << "(Jugador no ha atacado) El jugador " << jugador << " no ha ejecutado el comando atacar." << endl;
    return;
    }
    
    //  Verificar que sea el turno del jugador
    if (this->jugadorActual != jugadorActualObj) {
        cout << "No es el turno del jugador " << jugador << "." << endl;
        return;
    }
    
    //  Verificar que el jugador tenga al menos dos territorios
    const list<Territorio*>& territoriosJugador = jugadorActualObj->ObtenerTerritorios();
    if (territoriosJugador.size() < 2) {
        cout << "El jugador " << jugador << " necesita al menos 2 territorios para fortificar." << endl;
        return;
    }

    string nombreOrigen = territorio; //utilizamos esta variable para saber si tenemos un territorio directamente como parametro o tuvimos que cambiarlo
    
    // Si el territorio viene vacío desde la consola, se lo pedimos al jugador
    if (nombreOrigen.empty()) {
        cout << "Ingrese el nombre del territorio desde el cual desea mover unidades: ";
        getline(cin, nombreOrigen);
    }

    // Buscar el territorio de origen
    Territorio* territorioOrigen = BuscarTerritorio(nombreOrigen);
    if (territorioOrigen == nullptr) {
        cout << "El territorio " << nombreOrigen << " no existe en el tablero." << endl;
        return;
    }
    
    // Verificar que el territorio pertenezca al jugador
    if (territorioOrigen->ObtenerDueno
        () != jugadorActualObj) {
        cout << "El territorio " << nombreOrigen << " no pertenece al jugador " << jugador << "." << endl;
        return;
    }
    
    // Verificar que el territorio tenga más de 1 unidad
    if (territorioOrigen->ObtenerUnidades() <= 1) {
        cout << "El territorio " << nombreOrigen << " tiene " << territorioOrigen->ObtenerUnidades() 
             << " unidades, debe tener al menos 2 para poder fortificar." << endl;
        return;
    }
    
    // Mostrar los vecinos del territorio origen
    const list<Territorio*>& listaVecinos = territorioOrigen->ObtenerTerritoriosAdyacentes();
    vector<Territorio*> vecinos(listaVecinos.begin(), listaVecinos.end());
    vector<Territorio*> vecinosDisponibles;
    
    cout << "\n FORTIFICACIÓN " << endl;
    cout << "Territorio origen: " << territorioOrigen->ObtenerNombre() << " (" << territorioOrigen->ObtenerCodigo() << ")" << endl;
    cout << "Unidades disponibles para mover: " << territorioOrigen->ObtenerUnidades() - 1 << endl;
    cout << "\nTerritorios vecinos disponibles para fortificar:" << endl;
    
    // mostrar solo los que pertenecen al jugador
    for (size_t i = 0; i < vecinos.size(); i++) {
        Territorio* vecino = vecinos[i];
        if (vecino->ObtenerDueno
            () == jugadorActualObj) {
            vecinosDisponibles.push_back(vecino);
            cout << vecinosDisponibles.size() << ". " << vecino->ObtenerNombre() << " (" 
                 << vecino->ObtenerCodigo() << ") - Unidades: " << vecino->ObtenerUnidades() << endl;
        }
    }
    
    // Verificar si hay vecinos disponibles
    if (vecinosDisponibles.empty()) {
        cout << "(Fortificación inválida) No hay territorios vecinos pertenecientes al jugador " 
             << jugador << " para fortificar." << endl;
        return;
    }
    
    //  Solicitar selección del territorio destino
    int seleccionDestino;
    cout << "\nSeleccione el número del territorio destino (1-" << vecinosDisponibles.size() << "): ";
    if (!LeerEnteroConsola("", seleccionDestino)) {
        return;
    }
    
    if (seleccionDestino < 1 || seleccionDestino > static_cast<int>(vecinosDisponibles.size())) {
        cout << "(Selección inválida) Opción no válida." << endl;
        return;
    }
    
    Territorio* territorioDestino = vecinosDisponibles[seleccionDestino - 1];
    
    // Solicitar la cantidad de unidades a mover
    int maximoUnidades = territorioOrigen->ObtenerUnidades() - 1;
    int unidadesAMover;
    
    ostringstream mensajeUnidades;
    mensajeUnidades << "Cantidad de unidades a mover (1-" << maximoUnidades << "): ";
    if (!LeerEnteroConsola(mensajeUnidades.str(), unidadesAMover)) {
        return;
    }
    
    if (unidadesAMover < 1 || unidadesAMover > maximoUnidades) {
        cout << "(Cantidad inválida) Debe mover entre 1 y " << maximoUnidades << " unidades." << endl;
        return;
    }
    
    // Realizar la fortificación
    territorioOrigen->EliminarUnidades(unidadesAMover);
    territorioDestino->AgregarUnidades(unidadesAMover);
    
    // Mostrar resultado
    cout << "El jugador " << jugador 
         << " ha terminado de fortificar su posición." << endl;
    cout << "Movió " << unidadesAMover << " unidades desde " 
         << territorioOrigen->ObtenerNombre() << " hacia " 
         << territorioDestino->ObtenerNombre() << "." << endl;
    cout << "Unidades en " << territorioOrigen->ObtenerNombre() << ": " 
         << territorioOrigen->ObtenerUnidades() << endl;
    cout << "Unidades en " << territorioDestino->ObtenerNombre() << ": " 
         << territorioDestino->ObtenerUnidades() << endl;
    
    // Cambiar el turno después de fortificar
    CambiarTurno(jugador);
}

void Juego::AtacarTerritorio(const string &jugador, const string &territorio)
{
    // Inicio verificar casos error para validar que el jugador y territorio sean validos, que el juego este inicializado y que sea el turno del jugador
    if (!juegoInicializado)
    {
        cout << "(Juego no inicializado) El juego debe ser inicializado antes de realizar un ataque." << endl;
        return;
    }

    if (juegoTerminado){
        cout << "(Juego terminado) Esta partida ya tuvo un ganador." << endl;
        return;
    }

    Jugador *jugadorActualPtr = BuscarJugador(jugador);
    
    if (jugadorActualPtr == nullptr)
    {
        cout << "(Jugador no válido) El jugador " << jugador << " no existe en la partida." << endl;
        return;
    }

        //Verificar si ya ubicó unidades para que pueda atacar
    if (!jugadorActualPtr->ObtenerUnidadesReclamadas()) {
    cout << "(Jugador no ha ubicado unidades) El jugador " << jugador << " no ha ejecutado el comando obtener_unidades." << endl;
    return;
    }

    if (jugadorActual != jugadorActualPtr)
    {
        cout << "(Turno inválido) No es el turno del jugador " << jugador << "." << endl;
        return;
    }

    string nombreOrigen, nombreDestino;

    cout << "Ingrese el nombre del territorio desde el cual desea atacar: ";
    getline(cin, nombreOrigen);

    Territorio *origen = BuscarTerritorio(nombreOrigen);
    if (origen == nullptr)
    {
        cout << "(Territorio no válido) El territorio " << nombreOrigen << " no existe." << endl;
        return;
    }

    if (origen->ObtenerDueno
        () != jugadorActualPtr)
    {
        cout << "(Territorio no válido) El territorio " << nombreOrigen << " no pertenece al jugador " << jugador << "." << endl;
        return;
    }

    if (origen->ObtenerUnidades() <= 1)
    {
        cout << "(Unidades insuficientes) El territorio " << nombreOrigen << " no tiene suficientes unidades para atacar." << endl;
        return;
    }
    // Para dar continuidad al juego, si el territorio no se pasa como argumento, se le pide al jugador que lo ingrese por consola
    if (territorio.empty())
    {
        cout << "Ingrese el nombre del territorio al cual desea atacar: ";
        getline(cin, nombreDestino);
    }
    else
    {
        nombreDestino = territorio;
    }

    Territorio *destino = BuscarTerritorio(nombreDestino);
    if (destino == nullptr)
    {
        cout << "(Territorio no valido) El territorio " << nombreDestino << " no existe." << endl;
        return;
    }

    if (destino->ObtenerDueno
        () == jugadorActualPtr)
    {
        cout << "(Territorio no valido) El territorio " << nombreDestino << " ya pertenece al jugador " << jugador << "." << endl;
        return;
    }

    if (!origen->VerificarVecino(destino))
    {
        cout << "(Territorios no adyacentes) El territorio " << destino->ObtenerNombre() << " no es vecino de " << origen->ObtenerNombre() << "." << endl;
        return;
    }
    // fin casos error

    // muestra por consola que se va a realizar el ataque y de quien es el territorio que se va a atacar
    Jugador *defensor = destino->ObtenerDueno
    ();
    cout << "Comienza el ataque de " << origen->ObtenerNombre() << " contra " << destino->ObtenerNombre() << " (" << defensor->ObtenerNombre() << ")" << endl;

    bool continuarAtacando = true;

    jugadorActualPtr->EstablecerHaAtacado(true);

    while (continuarAtacando && origen->ObtenerUnidades() > 1 && destino->ObtenerUnidades() > 0)
    {
        int dadosAtacanteNum = min(3, origen->ObtenerUnidades() - 1); // El atacante puede lanzar hasta 3 dados, pero debe dejar al menos 1 unidad en el territorio
        int dadosDefensorNum = min(2, destino->ObtenerUnidades());    // El defensor puede lanzar hasta 2 dados, pero no más de las unidades que tiene

        vector<int> dadosAtacante;
        vector<int> dadosDefensor;
        dadosAtacante.reserve(dadosAtacanteNum);
        dadosDefensor.reserve(dadosDefensorNum);

        for (int i = 0; i < dadosAtacanteNum; i++)
            dadosAtacante.push_back((rand() % 6) + 1);
        for (int i = 0; i < dadosDefensorNum; i++)
            dadosDefensor.push_back((rand() % 6) + 1);

        sort(dadosAtacante.rbegin(), dadosAtacante.rend());
        sort(dadosDefensor.rbegin(), dadosDefensor.rend());

        cout << "Resultado de los dados: " << endl;
        cout << "Atacante (" << jugadorActualPtr->ObtenerNombre() << "): ";
        for (int d : dadosAtacante)
            cout << d << " ";
        cout << endl;
        cout << "Defensor (" << defensor->ObtenerNombre() << "): ";
        for (int d : dadosDefensor)
            cout << d << " ";
        cout << endl;

        int comparaciones = min(dadosAtacanteNum, dadosDefensorNum);
        int perdidasAtacante = 0;
        int perdidasDefensor = 0;

        for (int i = 0; i < comparaciones; i++)
        {
            if (dadosAtacante[i] > dadosDefensor[i])
            {
                perdidasDefensor++;
            }
            else
            {
                perdidasAtacante++;
            }
        }

        // Actualizacion de las unidades de los territorios y los ejércitos de los jugadores
        origen->EliminarUnidades(perdidasAtacante);
        jugadorActualPtr->EliminarEjercito(perdidasAtacante);
        destino->EliminarUnidades(perdidasDefensor);
        defensor->EliminarEjercito(perdidasDefensor);

        cout << "Unidades perdidas en esta ronda - Atacante: " << perdidasAtacante << ", Defensor: " << perdidasDefensor << endl;
        cout << "Unidades restantes - " << origen->ObtenerNombre() << ": " << origen->ObtenerUnidades() << ", " << destino->ObtenerNombre() << ": " << destino->ObtenerUnidades() << endl;

        if (destino->ObtenerUnidades() == 0)
        {
            cout << jugadorActualPtr->ObtenerNombre() << "!!! ha conquistado " << destino->ObtenerNombre() << endl;
            // Transferencia de propiedad del territorio
            defensor->EliminarTerritorio(destino);
            destino->CambiarDueno
            (jugadorActualPtr);
            jugadorActualPtr->AgregarTerritorio(destino);
            // El atacante debe mover al menos tantas unidades como dados lanzó pero tambien debe dejar al menos 1 unidad en el territorio de origen
            int tropasParaMover = min(dadosAtacanteNum, origen->ObtenerUnidades() - 1);
            origen->EliminarUnidades(tropasParaMover);
            destino->AgregarUnidades(tropasParaMover);

            cout << "Se han movido " << tropasParaMover << " unidades a " << destino->ObtenerNombre() << endl;

            if (defensor->ObtenerTerritorios().empty())
            {
                cout << "El jugador " << defensor->ObtenerNombre() << " ha sido eliminado del juego." << endl;
                EliminarJugadorDeJuego(defensor); // Elimina al jugadro de la lista
            }

            if (jugadorActualPtr->ObtenerTerritorios().size() == 42)
            {
                cout << "El jugador " << jugadorActualPtr->ObtenerNombre() << " ha conquistado todos los territorios y ha ganado el juego!" << endl;
                juegoTerminado = true; // Finaliza el juego
                return;
            }
            break; // Sale del bucle de ataque
        }

        string respuesta;
        cout << "¿Desea continuar atacando (s/n)? ";
        getline(cin, respuesta);
        if (respuesta != "s" && respuesta != "S")
        {
            cout << "El jugador " << jugadorActualPtr -> ObtenerNombre() << " ha terminado de atacar\n";
            continuarAtacando = false;
        }

    }
}
