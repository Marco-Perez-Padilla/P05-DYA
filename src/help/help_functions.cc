/**
** Universidad de La Laguna
** Escuela Superior de Ingenieria y Tecnologia
** Grado en Ingenieria Informatica
** Asignatura: Diseño y Analisis de Algoritmos
** Curso: 3º
** Practica 5: Algoritmos constructivos y búsquedas por entornos
** Autor: Marco Pérez Padilla
** Correo: alu0101469348@ull.edu.es
** Fecha: 31/03/2026

** Archivo help_functions.cc: Implementación de funciones auxiliares
**/

#include "help_functions.h"
#include <iostream>
#include <iomanip>

static constexpr int COL_LABEL   = 26;
static constexpr int COL_JOPEN   =  7;
static constexpr int COL_COST    = 14;
static constexpr int COL_INCOMP  =  9;
static constexpr int COL_TIME    = 11;
static constexpr int TABLE_WIDTH =
    COL_LABEL + COL_JOPEN + COL_COST * 3 + COL_INCOMP + COL_TIME + 2;


/**
 * @brief Shows the help message with usage instructions and options.
 */
void Help() {
  std::cout
    << "MS-CFLP-CI — Constructive & Local Search Algorithms\n\n"
    << "Uso: ./ms_cflp_ci\n\n"
    << "Opciones:\n"
    << "  --help, -h       Muestra este mensaje y termina\n";
}

/**
 * @brief Shows usage instructions when invalid arguments are provided.
 */
void Usage() {
  std::cout
    << "MS-CFLP-CI — Multi-Source Capacitated Facility Location Problem\n"
    << "            with Customer Incompatibilities\n\n"
    << "Resuelve el problema MS-CFLP-CI mediante algoritmos constructivos\n"
    << "(voraz y GRASP) y metaheurísticas de búsqueda local (GVNS).\n"
    << "Incluye VND con Reinforcement Learning (VND-RL) para la selección\n"
    << "dinámica de vecindades.\n\n"
    << "Algoritmos implementados:\n"
    << "  - Greedy Constructive (Voraz)\n"
    << "  - GRASP (Greedy Randomized Adaptive Search Procedure)\n"
    << "  - GVNS (General Variable Neighborhood Search)\n"
    << "    · Mejora: VND simple, RVND o VND-RL (configurable)\n\n"
    << "Búsquedas locales disponibles:\n"
    << "  - Shift (reinserción de cliente)\n"
    << "  - Swap Clientes (intercambio de clientes)\n"
    << "  - Swap Instalaciones (apertura/cierre de instalaciones)\n"
    << "  - Eliminación de Incompatibilidades\n\n"
    << "Estrategias de mejora:\n"
    << "  - VND (Variable Neighborhood Descent, orden fijo)\n"
    << "  - RVND (Random VND, orden aleatorio)\n"
    << "  - VND-RL (VND con Reinforcement Learning, política ε-greedy)\n\n"
    << "Uso:\n"
    << "  ./ms_cflp_ci\n\n"
    << "Opciones:\n"
    << "  --help, -h       Muestra este mensaje y termina\n\n"
    << "Instancias:\n"
    << "  Formato .dzn en data/input/Public/\n"
    << "  wlp01 a wlp20 (50-500 instalaciones, 115-1277 clientes)\n\n"
    << "Salida:\n"
    << "  - Tabla de resultados por consola\n"
    << "  - Fichero de resultados (configurable)\n"
    << "  - Fichero de evolución de Q (si se usa VND-RL)\n"
    << "  - Fichero de configuración de ejecución\n\n"
    << "Post-procesado:\n"
    << "  QTablesPostProcessing.py — Script Python para la visualización\n"
    << "  de la evolución de los valores Q del VND-RL.\n"
    << "  Ejecute 'python QTablesPostProcessing.py --help' para ver\n"
    << "  todas las opciones de generación de gráficas.\n\n";
}

/**
 * @brief Validates command-line arguments.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return  0 if --help was requested (terminate successfully)
 *         -1 if arguments are correct (continue to menus)
 *          1 if arguments are incorrect (terminate with error)
 */
int ValidateArguments(int argc, char* argv[]) {
  if (argc == 1) return -1;  // sin argumentos, se continúa a los menús
  if (argc == 2) {
    const std::string arg = argv[1];
    if (arg == "--help" || arg == "-h") {
      Help();
      return 0;
    }
  }
  Usage();
  return 1;
}

/**
 * @brief Prints the header of the results table.
 */
void printTableHeader() {
  std::cout << std::left
            << std::setw(COL_LABEL)  << "Algoritmo/Instancia"
            << std::setw(COL_JOPEN)  << "|Jopen|"
            << std::setw(COL_COST)   << "C.Fijo"
            << std::setw(COL_COST)   << "C.Asignacion"
            << std::setw(COL_COST)   << "C.Total"
            << std::setw(COL_INCOMP) << "Incomp."
            << std::setw(COL_TIME)   << "CPU(s)"
            << "\n"
            << std::string(TABLE_WIDTH, '-') << "\n";
}

/**
 * @brief Prints a row in the results table with the solution details.
 * @param label A string label describing the algorithm and instance.
 * @param solution The Solution object containing the results to display.
 * @param inst The Instance object providing context for incompatibility violations.
 * @param elapsed The time taken to compute the solution, in seconds.
 */
void printSolutionRow(const std::string& label, const Solution& solution, const Instance& inst, double elapsed) {
  std::cout << std::left  << std::setw(COL_LABEL)  << label
            << std::right << std::setw(COL_JOPEN)  << solution.openFacilitiesCount()
            << std::fixed << std::setprecision(2)
            << std::setw(COL_COST)   << solution.getFixedCost()
            << std::setw(COL_COST)   << solution.getTransportCost()
            << std::setw(COL_COST)   << solution.getTotalCost()
            << std::setw(COL_INCOMP) << solution.countIncompatibilityViolations(inst)
            << std::setprecision(4)
            << std::setw(COL_TIME)   << elapsed
            << "\n";
}