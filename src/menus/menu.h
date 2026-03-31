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

** Archivo menu.h: Clase Menu — orquestación de ejecución de algoritmos
**/

#pragma once
#include <string>
#include <memory>
#include <vector>
#include "../model/instance.h"
#include "../model/solution.h"
#include "../local_search/local_search.h"

/**
 * @brief Orquesta la carga de la instancia, la ejecución de algoritmos
 * (con medición de tiempos) y la presentación de resultados.
 *
 * El constructor carga la instancia; run() ejecuta el flujo completo.
 * Añadir un nuevo algoritmo solo requiere añadir un método privado run*()
 * y llamarlo desde run().
 */
class Menu {
 public:
  explicit Menu(const std::string& instancePath);

  /// Ejecuta todos los algoritmos configurados y presenta los resultados.
  void run();

 private:
  std::string instancePath_;
  std::string instanceName_;  ///< Solo el nombre del fichero (sin ruta)
  Instance    inst_;

  // ── Búsquedas locales compartidas ────────────────────────────────────────
  std::vector<std::shared_ptr<LocalSearch>> localSearchOps_;

  /// Aplica todas las búsquedas locales hasta alcanzar un óptimo local.
  void applyLocalSearches(Solution& sol) const;

  // ── Ejecuciones de algoritmos ─────────────────────────────────────────────
  void runGreedy();
  void runGRASP(int alpha, int iterations, unsigned seed);
};