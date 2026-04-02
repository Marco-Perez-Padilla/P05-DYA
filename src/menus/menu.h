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

#ifndef MENU_H
#define MENU_H

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
  bool run_greedy  = true;
  bool run_GRASP   = true;
  int  grasp_alpha = 3;
  int  grasp_iters = 100;
  std::vector<int> instance_indexes;  
};

/**
 * @brief Orquesta los menús interactivos, la carga de instancias y la
 * ejecución de algoritmos con medición de tiempos.
 */
class Menu {
 public:
  static void launch();

 private:
  std::string instance_path_;
  std::string instance_name_;
  Instance    inst_;
  RunConfig   config_;
  std::vector<std::shared_ptr<LocalSearch>> local_seach_ops_;

  Menu(const std::string& instance_path, const RunConfig& config);
  void run();
  static RunConfig askAlgorithmMenu();
  static void askInstanceMenu(RunConfig& config, const std::vector<std::string>& paths);
  static std::string askDataDir();
  static std::vector<std::string> findInstances(const std::string& dir);
  static void listInstances(const std::vector<std::string>& paths);
  static std::vector<int> parseSelection(const std::string& input, int total);
  void applyLocalSearches(Solution& solution) const;
  void runGreedyAlgo();
  void runGRASPAlgo();
};

#endif