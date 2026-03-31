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

#pragma once
#include <string>
#include <chrono>
#include "../model/instance.h"
#include "../model/solution.h"

// ── Validación de argumentos ──────────────────────────────────────────────────

void Help();
void Usage();

/**
 * @brief Valida los argumentos de línea de comandos.
 * @return  0 si se solicitó --help (terminar con éxito)
 *         -1 si los argumentos son correctos (continuar)
 *          1 si los argumentos son incorrectos (terminar con error)
 */
int  ValidateArguments(int argc, char* argv[]);

// ── Tabla de resultados ───────────────────────────────────────────────────────
void printTableHeader();
void printSolutionRow(const std::string& label,
                      const Solution&    sol,
                      const Instance&    inst,
                      double             elapsedSeconds);

// ── Temporizador simple ───────────────────────────────────────────────────────
struct Timer {
  std::chrono::high_resolution_clock::time_point t0 =
      std::chrono::high_resolution_clock::now();
  void reset() { t0 = std::chrono::high_resolution_clock::now(); }
  double elapsedSeconds() const {
    return std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now() - t0).count();
  }
};