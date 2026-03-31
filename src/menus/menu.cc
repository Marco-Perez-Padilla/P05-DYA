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

** Archivo menu.cc: Implementación de la clase Menu
**/

#include "menu.h"
#include "../io/instance_reader.h"
#include "../constructive/greedy/greedy_constructive.h"
#include "../constructive/grasp/grasp_constructive.h"
#include "../local_search/strategies/simple_local_search.h"
#include "../local_search/operators/shifts/shifts.h"
#include "../local_search/operators/swap_clients/swap_clients.h"
#include "../local_search/operators/swap_facilities/swap_facilities.h"
#include "../help/help_functions.h"
#include <iostream>
#include <filesystem>
#include <limits>

// ── Constructor ───────────────────────────────────────────────────────────────

Menu::Menu(const std::string& instancePath)
    : instancePath_(instancePath),
      instanceName_(std::filesystem::path(instancePath).filename().string()),
      inst_(InstanceReader::read(instancePath)) {

  // Construir las búsquedas locales compartidas una sola vez
  localSearchOps_ = {
    std::make_shared<Shifts>(),
    std::make_shared<SwapClients>(),
    std::make_shared<SwapFacilities>()
  };
}

// ── Ejecución principal ───────────────────────────────────────────────────────

void Menu::run() {
  std::cout << "Instancia : " << instanceName_ << "\n"
            << "Instalaciones (m): " << inst_.m
            << "   Clientes (n): "   << inst_.n
            << "   Incompatibilidades: " << inst_.numIncompatibilities
            << "\n\n";

  printTableHeader();

  // 1. Algoritmo voraz (sin búsqueda local)
  runGreedy();

  // 2. GRASP con búsqueda local — ajuste de parámetros básico
  for (int alpha : {2, 3}) {
    runGRASP(alpha, /*iterations=*/1, /*seed=*/42);
  }

  std::cout << "\n";
}

// ── Helpers privados ──────────────────────────────────────────────────────────

void Menu::applyLocalSearches(Solution& sol) const {
  bool improved = true;
  while (improved) {
    improved = false;
    for (auto& op : localSearchOps_)
      if (op->improve(sol, inst_)) improved = true;
  }

  // Comprobación de factibilidad obligatoria durante el desarrollo
#ifndef NDEBUG
  sol.checkFeasibilityAfterLS(inst_, "applyLocalSearches");
#endif
}

void Menu::runGreedy() {
  GreedyConstructive greedy;

  // ── Voraz sin búsqueda local ──────────────────────────────────────────
  Timer t;
  Solution sol = greedy.build(inst_);
  const double buildTime = t.elapsedSeconds();

  printSolutionRow(instanceName_ + " [voraz]", sol, inst_, buildTime);

  // ── Voraz + búsqueda local ────────────────────────────────────────────
  t.reset();
  applyLocalSearches(sol);
  const double lsTime = t.elapsedSeconds();

  printSolutionRow(instanceName_ + " [voraz+LS]", sol, inst_, buildTime + lsTime);
}

void Menu::runGRASP(int alpha, int iterations, unsigned seed) {
  GRASPConstructive grasp(alpha, seed);
  Solution bestSol;
  double   bestCost = std::numeric_limits<double>::max();

  Timer t;

  for (int iter = 0; iter < iterations; ++iter) {
    // Semilla distinta en cada iteración para explorar soluciones diferentes
    grasp.setSeed(seed + static_cast<unsigned>(iter));
    Solution sol = grasp.build(inst_);
    applyLocalSearches(sol);

    if (sol.getTotalCost() < bestCost) {
      bestCost = sol.getTotalCost();
      bestSol  = sol;
    }
  }

  const double elapsed = t.elapsedSeconds();

  const std::string label = instanceName_
      + " [GRASP α=" + std::to_string(alpha)
      + " i=" + std::to_string(iterations) + "]";

  printSolutionRow(label, bestSol, inst_, elapsed);
}