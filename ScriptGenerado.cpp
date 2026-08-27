#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm> // Para std::find y std::shuffle
#include <random>    // Para generación de números aleatorios

using namespace std;

// Estructura lineal para llevar el control de los jugadores
struct JugadorAux {
    string nombre;
    string color;
    int unidadesDisponibles;
};

// Estructura lineal para llevar el control de los territorios
struct TerritorioAux {
    string codigo;
    string colorDueño;
    int unidades;
};

int main() {
    cout << "=== Generador de Archivos de Inicio - RISK ===" << endl;

    int numJugadores;
    bool cantidadValida = false;

    // 1. Validar la cantidad de jugadores (Regla: 3 a 6)
    while (!cantidadValida) {
        cout << "Ingrese la cantidad de jugadores (3 a 6): ";
        cin >> numJugadores;

        if (numJugadores >= 3 && numJugadores <= 6) {
            cantidadValida = true;
        } else {
            cout << "Error: El juego soporta unicamente entre 3 y 6 jugadores." << endl;
        }
    }

    // 2. Calcular las unidades iniciales por jugador
    int unidadesMaximas = 0;
    if (numJugadores == 3) unidadesMaximas = 35;
    else if (numJugadores == 4) unidadesMaximas = 30;
    else if (numJugadores == 5) unidadesMaximas = 25;
    else if (numJugadores == 6) unidadesMaximas = 20;

    vector<JugadorAux> jugadores(numJugadores);
    vector<string> nombresUsados;
    vector<string> coloresUsados;

    // 3. Registrar jugadores validando unicidad y longitud (< 8 caracteres)
    for (int i = 0; i < numJugadores; i++) {
        bool jugadorValido = false;
        
        while (!jugadorValido) {
            string nombreTemp, colorTemp;
            cout << "\nJugador " << i + 1 << " - Ingrese el nombre (max 8 letras): ";
            cin >> nombreTemp;
            
            cout << "Jugador " << i + 1 << " - Ingrese el color (ej. Verde): ";
            cin >> colorTemp;

            // Validación A: Longitud del nombre
            if (nombreTemp.length() > 8) {
                cout << "-> Error: El nombre '" << nombreTemp << "' tiene mas de 8 caracteres. Intente de nuevo." << endl;
                continue;
            }

            // Validación B: Nombre único (Búsqueda lineal en el vector)
            bool nombreRepetido = false;
            for (const string& n : nombresUsados) {
                if (n == nombreTemp) nombreRepetido = true;
            }
            if (nombreRepetido) {
                cout << "-> Error: El nombre '" << nombreTemp << "' ya fue elegido por otro jugador." << endl;
                continue;
            }

            // Validación C: Color único (Búsqueda lineal en el vector)
            bool colorRepetido = false;
            for (const string& c : coloresUsados) {
                if (c == colorTemp) colorRepetido = true;
            }
            if (colorRepetido) {
                cout << "-> Error: El color '" << colorTemp << "' ya esta en uso." << endl;
                continue;
            }

            // Si pasa todas las validaciones, se agrega el jugador
            jugadores[i] = {nombreTemp, colorTemp, unidadesMaximas};
            nombresUsados.push_back(nombreTemp);
            coloresUsados.push_back(colorTemp);
            jugadorValido = true;
        }
    }

    // 4. Vector lineal estricto con los 42 códigos de territorio
    vector<string> codigosTerritorios = {
        "1.1", "1.2", "1.3", "1.4", "1.5", "1.6", "1.7", "1.8", "1.9",
        "2.1", "2.2", "2.3", "2.4",
        "3.1", "3.2", "3.3", "3.4", "3.5", "3.6", "3.7",
        "4.1", "4.2", "4.3", "4.4", "4.5", "4.6",
        "5.1", "5.2", "5.3", "5.4", "5.5", "5.6", "5.7", "5.8", "5.9", "5.10", "5.11", "5.12",
        "6.1", "6.2", "6.3", "6.4"
    };

    // Mezclar los territorios aleatoriamente para repartirlos
    random_device rd;
    mt19937 g(rd());
    shuffle(codigosTerritorios.begin(), codigosTerritorios.end(), g);

    vector<TerritorioAux> distribucion;
    
    // 5. Repartir los 42 territorios a los jugadores (1 unidad obligatoria para ocupar)
    int turno = 0;
    for (int i = 0; i < 42; i++) {
        string colorTurnoActual = jugadores[turno].color;
        distribucion.push_back({codigosTerritorios[i], colorTurnoActual, 1}); // Ocupa con 1 tropa
        
        jugadores[turno].unidadesDisponibles--; // Restamos la tropa utilizada
        
        turno++;
        if (turno == numJugadores) turno = 0; // Reiniciar el ciclo de turnos
    }

    // 6. Repartir el sobrante de tropas aleatoriamente en los territorios propios
    for (int i = 0; i < numJugadores; i++) {
        while (jugadores[i].unidadesDisponibles > 0) {
            // Elegimos un territorio al azar (de 0 a 41)
            int indiceAleatorio = g() % 42;
            
            // Si el territorio elegido pertenece al jugador actual, le ponemos 1 tropa más
            if (distribucion[indiceAleatorio].colorDueño == jugadores[i].color) {
                distribucion[indiceAleatorio].unidades++;
                jugadores[i].unidadesDisponibles--;
            }
        }
    }

    // 7. Generar el archivo txt con el formato exacto requerido
    ofstream archivoSalida("archivo_inicio.txt");

    if (!archivoSalida.is_open()) {
        cout << "\nError fatal: No se pudo crear el archivo_inicio.txt" << endl;
        return 1;
    }

    // Escribir N
    archivoSalida << numJugadores << "\n";

    // Escribir Nombres y Colores
    for (int i = 0; i < numJugadores; i++) {
        archivoSalida << jugadores[i].nombre << " " << jugadores[i].color << "\n";
    }

    // Ordenar los territorios de vuelta a su estado original (1.1 a 6.4) para mayor legibilidad
    // Utilizamos un algoritmo de ordenamiento simple (burbuja) sobre el vector de distribución
    for (size_t i = 0; i < distribucion.size() - 1; i++) {
        for (size_t j = 0; j < distribucion.size() - i - 1; j++) {
            // Comparación simple de strings (funciona porque los códigos están estructurados)
            if (distribucion[j].codigo > distribucion[j+1].codigo) {
                swap(distribucion[j], distribucion[j+1]);
            }
        }
    }

    // Escribir los 42 territorios ocupados
    for (int i = 0; i < 42; i++) {
        archivoSalida << distribucion[i].codigo << " " << distribucion[i].colorDueño << " " << distribucion[i].unidades << "\n";
    }

    archivoSalida.close();
    cout << "\n¡Exito! El archivo 'archivo_inicio.txt' ha sido generado y esta listo para la prueba." << endl;

    return 0;
}