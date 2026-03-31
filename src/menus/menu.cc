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
#include "../exceptions/exceptions.h"

#include <iostream>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <limits>

namespace fs = std::filesystem;

// ══════════════════════════════════════════════════════════════════════════════
// Punto de entrada
// ══════════════════════════════════════════════════════════════════════════════

void Menu::launch() {
  // 1. Preguntar directorio de instancias
  const std::string dir = askDataDir();
  const auto paths = findInstances(dir);
  if (paths.empty())
    throw FileNotFoundException(dir + " (ningún .dzn encontrado)");

  // 2. Menú de algoritmos
  RunConfig cfg = askAlgorithmMenu();

  // 3. Menú de instancias
  askInstanceMenu(cfg, paths);

  // 4. Ejecutar
  std::cout << "\n=== EJECUTANDO " << cfg.instanceIndices.size()
            << " INSTANCIA(S) ===\n\n";

  for (const int idx : cfg.instanceIndices) {
    std::cout << std::string(60, '-') << "\n";
    try {
      Menu menu(paths[idx], cfg);
      menu.run();
    } catch (const Exceptions& e) {
      std::cerr << "[Error en "
                << fs::path(paths[idx]).filename().string()
                << "] " << e.what() << "\n";
    }
  }
}

// ══════════════════════════════════════════════════════════════════════════════
// Menú 0 — directorio de datos
// ══════════════════════════════════════════════════════════════════════════════

std::string Menu::askDataDir() {
  const std::string defaultDir = "data/input/Public";
  std::cout << "\nDirectorio de instancias [" << defaultDir << "]: ";
  std::string line;
  std::getline(std::cin, line);
  // Eliminar espacios extremos
  line.erase(0, line.find_first_not_of(" \t"));
  line.erase(line.find_last_not_of(" \t") + 1);
  return line.empty() ? defaultDir : line;
}

// ══════════════════════════════════════════════════════════════════════════════
// Menú 1 — selección de algoritmos y parámetros GRASP
// ══════════════════════════════════════════════════════════════════════════════

RunConfig Menu::askAlgorithmMenu() {
  RunConfig cfg;
  std::string line;
  int op = 0;
  while (op < 1 || op > 3) {
    std::cout << "\n=== MENÚ DE ALGORITMOS ===\n"
              << "  [1] Solo Voraz  (Greedy + LS)\n"
              << "  [2] Solo GRASP  (GRASP  + LS)\n"
              << "  [3] Ambos\n"
              << "Opción [3]: ";

    int op = 3;
    if (std::getline(std::cin, line) && !line.empty())
      try { op = std::stoi(line); } catch (...) {
        op = 0;
      }

    if (op < 1 || op > 3) {
      std::cout << "  Opción no válida. Introduce 1, 2 o 3.\n";
    }
  }
  cfg.runGreedy = (op == 1 || op == 3);
  cfg.runGRASP  = (op == 2 || op == 3);

  if (cfg.runGRASP) {
    std::cout << "\n  Tamaño LRC alpha [3]: ";
    if (std::getline(std::cin, line) && !line.empty())
      try { cfg.graspAlpha = std::stoi(line); } catch (...) {}

    std::cout << "  Iteraciones GRASP [100]: ";
    if (std::getline(std::cin, line) && !line.empty())
      try { cfg.graspIters = std::stoi(line); } catch (...) {}
  }

  return cfg;
}

// ══════════════════════════════════════════════════════════════════════════════
// Menú 2 — selección de instancias
// ══════════════════════════════════════════════════════════════════════════════

void Menu::askInstanceMenu(RunConfig& cfg, const std::vector<std::string>& paths) {
  listInstances(paths);

  std::cout << "\n=== MENÚ DE INSTANCIAS ===\n"
            << "  Formato: all | 1 | 1-5 | 1,3,7 | 2-4,8\n"
            << "Selección [all]: ";

  std::string sel = "all";
  std::string line;
  if (std::getline(std::cin, line) && !line.empty()) sel = line;

  cfg.instanceIndices = parseSelection(sel, static_cast<int>(paths.size()));

  if (cfg.instanceIndices.empty()) {
    std::cout << "  (selección vacía — se usan todas)\n";
    cfg.instanceIndices.resize(paths.size());
    for (int i = 0; i < (int)paths.size(); ++i) cfg.instanceIndices[i] = i;
  }
}

// ══════════════════════════════════════════════════════════════════════════════
// Constructor privado
// ══════════════════════════════════════════════════════════════════════════════

Menu::Menu(const std::string& instancePath, const RunConfig& cfg)
    : instancePath_(instancePath),
      instanceName_(fs::path(instancePath).filename().string()),
      inst_(InstanceReader::read(instancePath)),
      cfg_(cfg) {
  localSearchOps_ = {
    std::make_shared<Shifts>(),
    std::make_shared<SwapClients>(),
    std::make_shared<SwapFacilities>()
  };
}

// ══════════════════════════════════════════════════════════════════════════════
// Ejecución por instancia
// ══════════════════════════════════════════════════════════════════════════════

void Menu::run() {
  std::cout << "Instancia : " << instanceName_ << "\n"
            << "Instalaciones (m): " << inst_.m
            << "   Clientes (n): "   << inst_.n
            << "   Incompatibilidades: " << inst_.numIncompatibilities
            << "\n\n";

  printTableHeader();

  if (cfg_.runGreedy) runGreedyAlgo();
  if (cfg_.runGRASP)  runGRASPAlgo();

  std::cout << "\n";
}

// ══════════════════════════════════════════════════════════════════════════════
// Algoritmos
// ══════════════════════════════════════════════════════════════════════════════

void Menu::applyLocalSearches(Solution& sol) const {
  bool improved = true;
  while (improved) {
    improved = false;
    for (auto& op : localSearchOps_)
      if (op->improve(sol, inst_)) improved = true;
  }
#ifndef NDEBUG
  sol.checkFeasibilityAfterLS(inst_, "applyLocalSearches");
#endif
}

void Menu::runGreedyAlgo() {
  GreedyConstructive greedy;
  Timer t;
  Solution sol = greedy.build(inst_);
  printSolutionRow(instanceName_ + " [voraz]", sol, inst_, t.elapsedSeconds());

  t.reset();
  applyLocalSearches(sol);
  printSolutionRow(instanceName_ + " [voraz+LS]", sol, inst_, t.elapsedSeconds());
}

void Menu::runGRASPAlgo() {
  // Cada iteración resiembra con el reloj → soluciones constructivas distintas.
  // Con muchas iteraciones se exploran cuencas de atracción diferentes;
  // la LS las profundiza y se reporta la mejor encontrada.
  GRASPConstructive grasp(cfg_.graspAlpha);
  Solution bestSol;
  double   bestCost = std::numeric_limits<double>::max();

  Timer t;
  for (int iter = 0; iter < cfg_.graspIters; ++iter) {
    grasp.reseed();
    Solution sol = grasp.build(inst_);
    applyLocalSearches(sol);
    if (sol.getTotalCost() < bestCost) {
      bestCost = sol.getTotalCost();
      bestSol  = sol;
    }
  }

  const std::string label = instanceName_
      + " [GRASP α=" + std::to_string(cfg_.graspAlpha)
      + " i="        + std::to_string(cfg_.graspIters) + "]";
  printSolutionRow(label, bestSol, inst_, t.elapsedSeconds());
}

// ══════════════════════════════════════════════════════════════════════════════
// Utilidades de instancias
// ══════════════════════════════════════════════════════════════════════════════

std::vector<std::string> Menu::findInstances(const std::string& dir) {
  std::vector<std::string> paths;
  if (!fs::is_directory(dir)) return paths;
  for (const auto& e : fs::directory_iterator(dir))
    if (e.path().extension() == ".dzn")
      paths.push_back(e.path().string());
  std::sort(paths.begin(), paths.end());
  return paths;
}

void Menu::listInstances(const std::vector<std::string>& paths) {
  std::cout << "\nInstancias disponibles:\n";
  for (size_t i = 0; i < paths.size(); ++i)
    std::cout << "  [" << (i + 1) << "] "
              << fs::path(paths[i]).filename().string() << "\n";
}

std::vector<int> Menu::parseSelection(const std::string& input, int total) {
  std::vector<int> selected;

  if (input == "all") {
    selected.resize(total);
    for (int i = 0; i < total; ++i) selected[i] = i;
    return selected;
  }

  std::stringstream ss(input);
  std::string token;
  while (std::getline(ss, token, ',')) {
    token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
    if (token.empty()) continue;
    const size_t dash = token.find('-');
    if (dash != std::string::npos) {
      try {
        const int a = std::stoi(token.substr(0, dash));
        const int b = std::stoi(token.substr(dash + 1));
        for (int x = a; x <= b; ++x)
          if (x >= 1 && x <= total) selected.push_back(x - 1);
      } catch (...) {}
    } else {
      try {
        const int x = std::stoi(token);
        if (x >= 1 && x <= total) selected.push_back(x - 1);
      } catch (...) {}
    }
  }

  std::sort(selected.begin(), selected.end());
  selected.erase(std::unique(selected.begin(), selected.end()), selected.end());
  return selected;
}