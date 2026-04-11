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
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      31/03/2026 - Creacion (primera version) del codigo
**      11/04/2026 - Modificación para añadir opciones de estrategias
**/

#include "menu.h"
#include "../io/instance_reader.h"
#include "../constructive/greedy/greedy_constructive.h"
#include "../constructive/grasp/grasp_constructive.h"
#include "../local_search/strategies/simple_local_search.h"
#include "../local_search/operators/shifts/shifts.h"
#include "../local_search/operators/swap_clients/swap_clients.h"
#include "../local_search/operators/swap_facilities/swap_facilities.h"
#include "../local_search/strategies/rvnd.h"
#include "../local_search/operators/incomp_elimination/incomp_elimination.h"
#include "../help/help_functions.h"
#include "../exceptions/exceptions.h"

#include <iostream>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <limits>

/**
 * @brief Constructor of the Menu class that initializes the instance path, name, loads the instance data, and sets up the local search operators.
 * @param instance_path The file path to the instance data.
 * @param config The RunConfig containing the user's algorithm and instance selections.
 */
Menu::Menu(const std::string& instance_path, const RunConfig& config)
    : instance_path_(instance_path),
      instance_name_(std::filesystem::path(instance_path).filename().string()),
      inst_(InstanceReader::read(instance_path)),
      config_(config) {

  local_seach_ops_.clear();
  if (config_.use_operators.size() >= 4) {
    if (config_.use_operators[0]) local_seach_ops_.push_back(std::make_shared<Shifts>());
    if (config_.use_operators[1]) local_seach_ops_.push_back(std::make_shared<SwapClients>());
    if (config_.use_operators[2]) local_seach_ops_.push_back(std::make_shared<SwapFacilities>());
    if (config_.use_operators[3]) local_seach_ops_.push_back(std::make_shared<IncompElimination>());
  }
}

/**
 * @brief Launches the interactive menu for selecting data directory, algorithms, and instances, and then executes the selected algorithms on the chosen instances with timing and result reporting.
 */
void Menu::launch() {
  const std::string dir = askDataDir();
  const auto paths = findInstances(dir);
  if (paths.empty()) {
    throw FileNotFoundException(dir + " (ningún .dzn encontrado)");
  }

  RunConfig config = askAlgorithmMenu();
  askInstanceMenu(config, paths);

  std::cout << "\n=== EJECUTANDO " << config.instance_indexes.size() << " INSTANCIA(S) ===\n\n";
  for (const int index : config.instance_indexes) {
    std::cout << std::string(60, '-') << "\n";
    try {
      Menu menu(paths[index], config);
      menu.run();
    } catch (const Exceptions& error) {
      std::cerr << "[Error en " << std::filesystem::path(paths[index]).filename().string() << "] " << error.what() << "\n";
    }
  }
}

/**
 * @brief Asks the user for the data directory containing instance files, with a default option. Trims whitespace and validates input.
 * @return The path to the data directory as a string.
 */
std::string Menu::askDataDir() {
  const std::string defaultDir = "data/input/Public";
  std::cout << "\nDirectorio de instancias [" << defaultDir << "]: ";
  std::string line;
  std::getline(std::cin, line);
  line.erase(0, line.find_first_not_of(" \time"));
  line.erase(line.find_last_not_of(" \time") + 1);
  return line.empty() ? defaultDir : line;
}

/**
 * @brief Displays a menu for selecting which algorithms to run (Greedy, GRASP, or both) and allows configuring GRASP parameters if selected.
 * @return A RunConfig struct containing the user's selections for algorithms and parameters.
 */
RunConfig Menu::askAlgorithmMenu() {
  RunConfig config;
  std::string line;
  int op = 0;
  
  while (op < 1 || op > 3) {
    std::cout << "\n=== MENÚ DE ALGORITMOS CONSTRUCTIVOS ===\n"
              << "  [1] Solo Voraz\n"
              << "  [2] Solo GRASP\n"
              << "  [3] Ambos\n"
              << "Opción [3]: ";

    op = 3;
    if (std::getline(std::cin, line) && !line.empty())
      try { op = std::stoi(line); } catch (...) { op = 0; }

    if (op < 1 || op > 3)
      std::cout << "  Opción no válida. Introduce 1, 2 o 3.\n";
  }
  config.run_greedy = (op == 1 || op == 3);
  config.run_GRASP  = (op == 2 || op == 3);

  if (config.run_greedy) {
    config.greedy_ls_mode = askLSMode("Voraz");
  }

  if (config.run_GRASP) {
    config.grasp_ls_mode = askLSMode("GRASP");

    std::cout << "\n  Tamaño LRC alpha [3]: ";
    if (std::getline(std::cin, line) && !line.empty()) {
      try { config.grasp_alpha = std::stoi(line); } catch (...) {}
    }

    std::cout << "  Iteraciones GRASP [100]: ";
    if (std::getline(std::cin, line) && !line.empty()) {
      try { config.grasp_iters = std::stoi(line); } catch (...) {}
    }
  }

  // Determinar si se necesita configurar búsqueda local
  bool need_ls_config = false;
  if (config.run_greedy && (config.greedy_ls_mode == LSMode::ONLY_LS || config.greedy_ls_mode == LSMode::BOTH))
    need_ls_config = true;
  if (config.run_GRASP && (config.grasp_ls_mode == LSMode::ONLY_LS || config.grasp_ls_mode == LSMode::BOTH))
    need_ls_config = true;

  if (need_ls_config) {
    askLocalSearchMenu(config);
  } else {
    // Inicializar use_operators con 4 false para evitar segmentation fault en constructor
    config.use_operators.assign(4, false);
    config.use_rvnd = false;
    config.rvnd_order.clear();
  }

  return config;
}

/**
 * @brief Displays a menu for selecting which instances to run on, allowing selection by index, range, or all. Validates input and updates the RunConfig with the selected instance indexes.
 * @param config The RunConfig struct to update with the selected instance indexes.
 * @param paths The list of available instance file paths to select from.
 */
void Menu::askInstanceMenu(RunConfig& config, const std::vector<std::string>& paths) {
  listInstances(paths);

  std::cout << "\n=== MENÚ DE INSTANCIAS ===\n"
            << "  Formato: all | 1 | 1-5 | 1,3,7 | 2-4,8\n"
            << "Selección [all]: ";

  std::string sel = "all";
  std::string line;
  if (std::getline(std::cin, line) && !line.empty()) {
    sel = line;
  }

  config.instance_indexes = parseSelection(sel, static_cast<int>(paths.size()));

  if (config.instance_indexes.empty()) {
    std::cout << "  (selección vacía — se usan todas)\n";
    config.instance_indexes.resize(paths.size());
    for (int i = 0; i < (int)paths.size(); ++i) {
      config.instance_indexes[i] = i;
    }
  }
}

/**
 * @brief Executes the selected algorithms on the loaded instance, applying local search and printing results with timing. It runs the Greedy algorithm if selected, followed by GRASP if selected, and applies local search to each solution before reporting.
 * The results include the instance name, algorithm label, solution cost, and execution time. If any algorithm produces an infeasible solution after local search, an exception is thrown with details about the infe
 */
void Menu::run() {
  std::cout << "Instancia : " << instance_name_ << "\n"
            << "Instalaciones (warehouses): " << inst_.warehouses
            << "   Clientes (stores): "   << inst_.stores
            << "   Incompatibilidades: " << inst_.num_incompatibilities
            << "\n\n";

  printTableHeader();

  if (config_.run_greedy) runGreedyAlgo();
  if (config_.run_GRASP)  runGRASPAlgo();

  std::cout << "\n";
}

/**
 * @brief Applies the configured local search operators iteratively to the given solution until no further improvements can be made. After applying local search, it checks the feasibility of the solution and throws an exception if it is infeasible.
 * @param solution The Solution object to improve using local search.
 */
void Menu::applyLocalSearches(Solution& solution) const {
  if (local_seach_ops_.empty()) return;

  if (config_.use_rvnd) {
    RVND rvnd(local_seach_ops_);
    if (!config_.rvnd_order.empty()) rvnd.setOrder(config_.rvnd_order);
    rvnd.improve(solution, inst_);
  } else {
    SimpleLocalSearch sls(local_seach_ops_);
    if (!config_.rvnd_order.empty()) sls.setOrder(config_.rvnd_order);
    sls.improve(solution, inst_);
  }
  solution.checkFeasibilityAfterLS(inst_, "applyLocalSearches");
}

/**
 * @brief Runs the Greedy algorithm on the loaded instance and applies local search.
 */
void Menu::runGreedyAlgo() {
  GreedyConstructive greedy;
  Timer time;
  Solution solution = greedy.build(inst_);
  std::string label = instance_name_ + " [voraz]";

  bool show_without_ls = (config_.greedy_ls_mode == LSMode::NONE || config_.greedy_ls_mode == LSMode::BOTH);
  bool show_with_ls    = (config_.greedy_ls_mode == LSMode::ONLY_LS || config_.greedy_ls_mode == LSMode::BOTH);

  if (show_without_ls) {
    printSolutionRow(label, solution, inst_, time.elapsedSeconds());
  }

  if (show_with_ls) {
    time.reset();
    applyLocalSearches(solution);
    printSolutionRow(instance_name_ + " [voraz+LS]", solution, inst_, time.elapsedSeconds());
  }
}

/**
 * @brief Runs the GRASP algorithm on the loaded instance with the configured parameters and applies local search. It performs multiple iterations of GRASP, keeping track of the best solution found across all iterations, and reports the best solution at the end.
 * Each iteration of GRASP is reseeded with the current time to explore different random solutions, and local search is applied to each solution to find local optima. The final reported solution is the
 */
void Menu::runGRASPAlgo() {
  GRASPConstructive grasp(config_.grasp_alpha);
  
  bool show_without_ls = (config_.grasp_ls_mode == LSMode::NONE || config_.grasp_ls_mode == LSMode::BOTH);
  bool show_with_ls    = (config_.grasp_ls_mode == LSMode::ONLY_LS || config_.grasp_ls_mode == LSMode::BOTH);

  if (show_without_ls) {
    Solution best_without;
    double best_cost_without = std::numeric_limits<double>::max();
    Timer time_without;
    for (int iter = 0; iter < config_.grasp_iters; ++iter) {
      grasp.reseed();
      Solution solution = grasp.build(inst_);
      if (solution.getTotalCost() < best_cost_without) {
        best_cost_without = solution.getTotalCost();
        best_without = solution;
      }
    }
    std::string label = instance_name_
        + " [GRASP α=" + std::to_string(config_.grasp_alpha)
        + " i=" + std::to_string(config_.grasp_iters) + "]";
    printSolutionRow(label, best_without, inst_, time_without.elapsedSeconds());
  }

  if (show_with_ls) {
    Solution best_with;
    double best_cost_with = std::numeric_limits<double>::max();
    Timer time_with;
    for (int iter = 0; iter < config_.grasp_iters; ++iter) {
      grasp.reseed();
      Solution solution = grasp.build(inst_);
      applyLocalSearches(solution); 
      if (solution.getTotalCost() < best_cost_with) {
        best_cost_with = solution.getTotalCost();
        best_with = solution;
      }
    }
    std::string label = instance_name_
        + " [GRASP α=" + std::to_string(config_.grasp_alpha)
        + " i=" + std::to_string(config_.grasp_iters) + " +LS]";
    printSolutionRow(label, best_with, inst_, time_with.elapsedSeconds());
  }
}

/**
 * @brief Finds all instance files with a .dzn extension in the given directory and returns their paths as a vector of strings. If the directory does not exist or contains no .dzn files, an empty vector is returned.
 * @param dir The directory to search for instance files.
 * @return A vector of strings containing the file paths of all .dzn instance files found
 */
std::vector<std::string> Menu::findInstances(const std::string& dir) {
  std::vector<std::string> paths;
  if (!std::filesystem::is_directory(dir)) return paths;
  for (const auto& entry : std::filesystem::directory_iterator(dir)) {
    if (entry.path().extension() == ".dzn") {
      paths.push_back(entry.path().string());
    }
  }
  std::sort(paths.begin(), paths.end());
  return paths;
}

/**
 * @brief Displays a numbered list of available instance files to the user, showing only the file names without paths. This is used in the instance selection menu to help the user choose which instances to run.
 * @param paths A vector of strings containing the file paths of available instance files.
 */
void Menu::listInstances(const std::vector<std::string>& paths) {
  std::cout << "\nInstancias disponibles:\n";
  for (size_t i = 0; i < paths.size(); ++i) {
    std::cout << "  [" << (i + 1) << "] " << std::filesystem::path(paths[i]).filename().string() << "\n";
  }
}

/**
 * @brief Parses the user's input for instance selection, allowing for individual indexes, ranges, or "all". It validates the input and returns a vector of selected instance indexes (0-based) based on the total number of available instances.
 * @param input The user's input string for instance selection.
 * @param total The total number of available instances to validate against.
 * @return A vector of integers representing the selected instance indexes (0-based).
 */
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
        for (int demand_fraction = a; demand_fraction <= b; ++demand_fraction) {
          if (demand_fraction >= 1 && demand_fraction <= total) {
            selected.push_back(demand_fraction - 1);
          }
        }
      } catch (...) {}
    } else {
      try {
        const int demand_fraction = std::stoi(token);
        if (demand_fraction >= 1 && demand_fraction <= total) {
          selected.push_back(demand_fraction - 1);
        }
      } catch (...) {}
    }
  }

  std::sort(selected.begin(), selected.end());
  selected.erase(std::unique(selected.begin(), selected.end()), selected.end());
  return selected;
}

/**
 * @brief Asks the user to select the local search mode for a given algorithm (None, Only LS, or Both) and returns the corresponding LSMode enum value based on the user's choice. The default option is "Only LS".
 * @param algorithmName The name of the algorithm for which the local search mode is being configured
 * @return The selected LSMode enum value representing the local search mode for the algorithm.
 */
LSMode Menu::askLSMode(const std::string& algorithmName) {
  std::cout << "\nModo de búsqueda local para " << algorithmName << ":\n"
            << "  [1] Sin búsqueda local (solo constructivo)\n"
            << "  [2] Con búsqueda local\n"
            << "  [3] Ambos (mostrar sin y con LS)\n"
            << "Opción [2]: ";
  std::string line;
  int choice = 2;
  if (std::getline(std::cin, line) && !line.empty()) {
    try { choice = std::stoi(line); } catch (...) { choice = 2; }
  }
  if (choice == 1) return LSMode::NONE;
  if (choice == 3) return LSMode::BOTH;
  return LSMode::ONLY_LS;  // default 2
}

/**
 * @brief Displays a menu for configuring local search options, including the choice between VND and RVND strategies, which operators to include, and the order of neighborhoods. The user's selections are stored in the provided RunConfig struct.
 * The menu allows the user to enable or disable each of the four local search operators (Shift, SwapClients, SwapFacilities, IncompElimination) and to specify the order in which they should be applied. If RVND is selected, the order will be used as the initial order and then randomized in each iteration.
 * If no operators are selected, the local search configuration will be effectively disabled.
 * @param config The RunConfig struct to update with the user's local search configuration selections.
 */
void Menu::askLocalSearchMenu(RunConfig& config) {
  std::string line;

  std::cout << "\n=== ESTRATEGIA DE BÚSQUEDA LOCAL ===\n"
            << "  [1] VND (orden fijo de vecindades)\n"
            << "  [2] RVND (orden aleatorio en cada iteración)\n"
            << "Opción [1]: ";
  int ls_choice = 1;
  if (std::getline(std::cin, line) && !line.empty()) {
    try { ls_choice = std::stoi(line); } catch (...) {}
  }
  config.use_rvnd = (ls_choice == 2);

  std::cout << "\nSeleccione los operadores a incluir (1=Sí, 0=No):\n";
  config.use_operators.resize(4, true);
  const std::vector<std::string> op_names = {
    "Shift", "SwapClientes", "SwapInstalaciones", "EliminacionIncomp"
  };
  for (size_t i = 0; i < op_names.size(); ++i) {
    std::cout << "  Incluir " << op_names[i] << "? [1]: ";
    std::string ans;
    if (std::getline(std::cin, ans) && !ans.empty()) {
      try { config.use_operators[i] = (std::stoi(ans) != 0); } catch (...) {}
    }
  }

  int num_selected = std::count(config.use_operators.begin(), config.use_operators.end(), true);
  if (num_selected > 0) {
    std::cout << "\nOrden de vecindades (números 1-4 separados por espacios):\n"
              << "  1:Shift  2:SwapClientes  3:SwapInstalaciones  4:EliminacionIncomp\n";
    if (config.use_rvnd) {
      std::cout << "Nota: En RVND este orden se usará como inicial y luego se barajará aleatoriamente.\n";
    }
    std::cout << "Orden [";
    bool first = true;
    for (int i = 0; i < 4; ++i) {
      if (config.use_operators[i]) {
        if (!first) std::cout << " ";
        std::cout << (i+1);
        first = false;
      }
    }
    std::cout << "]: ";

    std::string order_line;
    bool valid_order = false;
    while (!valid_order) {
      if (!std::getline(std::cin, order_line)) break; // EOF
      std::stringstream ss(order_line);
      int idx;
      std::vector<int> temp_order;
      bool invalid_input = false;
      while (ss >> idx) {
        if (idx < 1 || idx > 4 || !config.use_operators[idx-1]) {
          invalid_input = true;
          break;
        }
        if (std::find(temp_order.begin(), temp_order.end(), idx-1) != temp_order.end()) {
          invalid_input = true;
          break;
        }
        temp_order.push_back(idx-1);
      }
      if (!invalid_input && (int)temp_order.size() == num_selected) {
        config.rvnd_order = std::move(temp_order);
        valid_order = true;
      } else {
        std::cout << "  Orden inválido. Debe contener exactamente los números de los operadores seleccionados (";
        first = true;
        for (int i = 0; i < 4; ++i) {
          if (config.use_operators[i]) {
            if (!first) std::cout << ", ";
            std::cout << (i+1);
            first = false;
          }
        }
        std::cout << ") sin repeticiones.\n";
        std::cout << "  Inténtelo de nuevo: ";
      }
    }
    if (!valid_order) {
      config.rvnd_order.clear();
      for (int i = 0; i < 4; ++i)
        if (config.use_operators[i]) config.rvnd_order.push_back(i);
      std::cout << "\n  Usando orden por defecto.\n";
    }
  } else {
    config.rvnd_order.clear();
  }
}