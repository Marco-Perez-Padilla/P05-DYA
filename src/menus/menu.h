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

** Archivo menu.h: Clase Menu
**/

#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../model/instance.h"
#include "../model/solution.h"
#include "../local_search/local_search.h"

/**
 * @brief Configuración de ejecución elegida por el usuario en los menús.
 */
struct RunConfig {
  bool runGreedy  = true;
  bool runGRASP   = true;
  int  graspAlpha = 3;
  int  graspIters = 100;
  std::vector<int> instanceIndices;  ///< índices base-0 de las instancias a ejecutar
};

/**
 * @brief Orquesta los menús interactivos, la carga de instancias y la
 * ejecución de algoritmos con medición de tiempos.
 *
 * Uso externo: Menu::launch() desde main(), sin argumentos.
 */
class Menu {
 public:
  /// Punto de entrada: muestra los menús y ejecuta los algoritmos.
  static void launch();

 private:
  // ── Constructor (uso interno) ─────────────────────────────────────────────
  Menu(const std::string& instancePath, const RunConfig& cfg);

  /// Ejecuta los algoritmos configurados sobre la instancia cargada.
  void run();

  // ── Menús interactivos ────────────────────────────────────────────────────
  static RunConfig askAlgorithmMenu();
  static void      askInstanceMenu(RunConfig& cfg,
                                   const std::vector<std::string>& paths);

  // ── Utilidades de instancias ──────────────────────────────────────────────
  static std::string              askDataDir();
  static std::vector<std::string> findInstances(const std::string& dir);
  static void                     listInstances(const std::vector<std::string>& paths);
  static std::vector<int>         parseSelection(const std::string& input, int total);

  // ── Datos de instancia y configuración ───────────────────────────────────
  std::string instancePath_;
  std::string instanceName_;
  Instance    inst_;
  RunConfig   cfg_;

  // ── Búsquedas locales compartidas ────────────────────────────────────────
  std::vector<std::shared_ptr<LocalSearch>> localSearchOps_;
  void applyLocalSearches(Solution& sol) const;

  // ── Algoritmos ────────────────────────────────────────────────────────────
  void runGreedyAlgo();
  void runGRASPAlgo();
};