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

** Archivo help_functions.h: Funciones auxiliares, Timer y presentación de resultados
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

/// Comprueba que el nombre de fichero tenga extensión .dzn
bool ValidateDataFile(const std::string& name);

// ── Presentación de soluciones ────────────────────────────────────────────────

/**
 * @brief Imprime una cabecera de tabla de resultados por stdout.
 */
void printTableHeader();

/**
 * @brief Imprime una fila de resultados para una solución.
 * @param label     Etiqueta descriptiva (algoritmo + instancia)
 * @param sol       Solución a presentar
 * @param inst      Instancia del problema
 * @param elapsed   Tiempo de CPU en segundos
 */
void printSolutionRow(const std::string& label,
                      const Solution&   sol,
                      const Instance&   inst,
                      double            elapsed);

/**
 * @brief Imprime un resumen detallado de una solución (bloque de texto).
 * @param label  Etiqueta descriptiva
 * @param sol    Solución a presentar
 * @param inst   Instancia del problema
 */
void printSolution(const std::string& label,
                   const Solution&    sol,
                   const Instance&    inst);

// ── Temporizador ─────────────────────────────────────────────────────────────

/**
 * @brief Temporizador RAII de alta resolución basado en std::chrono.
 *
 * Uso típico:
 *   Timer t;
 *   // ... código a medir ...
 *   double secs = t.elapsedSeconds();
 */
class Timer {
 public:
  Timer() { reset(); }

  /// Reinicia el contador
  void reset() { start_ = std::chrono::steady_clock::now(); }

  /// Segundos transcurridos desde la construcción o el último reset()
  double elapsedSeconds() const {
    return std::chrono::duration<double>(
               std::chrono::steady_clock::now() - start_)
               .count();
  }

 private:
  std::chrono::steady_clock::time_point start_;
};