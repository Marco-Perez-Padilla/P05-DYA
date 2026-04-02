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

** Archivo help_functions.h: funciones de presentación de resultados
**/

#ifndef HELP_FUNCTIONS_H
#define HELP_FUNCTIONS_H

#include <string>
#include <chrono>
#include "../model/instance.h"
#include "../model/solution.h"


void Help();
void Usage();
int  ValidateArguments(int argc, char* argv[]);
void printTableHeader();
void printSolutionRow(const std::string& label, const Solution& solution, const Instance& inst, double elapsedSeconds);

struct Timer {
  std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();
  void reset() {t0 = std::chrono::high_resolution_clock::now();}
  double elapsedSeconds() const {return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t0).count();}
};

#endif