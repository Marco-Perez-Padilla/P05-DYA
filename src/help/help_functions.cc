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


void Help() {
  std::cout
    << "MS-CFLP-CI — Constructive & Local Search Algorithms\n\n"
    << "Uso: ./ms_cflp_ci\n\n"
    << "Opciones:\n"
    << "  --help, -h       Muestra este mensaje y termina\n";
}

void Usage() {
  std::cout
    << "Uso: ./ms_cflp_ci\n"
    << "     ./ms_cflp_ci --help\n";
}

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

void printSolutionRow(const std::string& label,
                      const Solution&    sol,
                      const Instance&    inst,
                      double             elapsed) {
  std::cout << std::left  << std::setw(COL_LABEL)  << label
            << std::right << std::setw(COL_JOPEN)  << sol.openFacilitiesCount()
            << std::fixed << std::setprecision(2)
            << std::setw(COL_COST)   << sol.getFixedCost()
            << std::setw(COL_COST)   << sol.getTransportCost()
            << std::setw(COL_COST)   << sol.getTotalCost()
            << std::setw(COL_INCOMP) << sol.countIncompatibilityViolations(inst)
            << std::setprecision(4)
            << std::setw(COL_TIME)   << elapsed
            << "\n";
}