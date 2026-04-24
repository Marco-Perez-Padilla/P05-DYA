/**
** Universidad de La Laguna
** Escuela Superior de Ingenieria y Tecnologia
** Grado en Ingenieria Informatica
** Asignatura: Diseño y Analisis de Algoritmos
** Curso: 3º
** Practica 5: Algoritmos constructivos y búsquedas por entornos
** Autor: Marco Pérez Padilla
** Correo: alu0101469348@ull.edu.es
** Fecha: 24/04/2026

** Archivo menu_coordinator.cc: Implementación de las funcionas que coordinan la ejecución de los menús y algoritmos
**                              Resultado de la refactorización para mejorar la organización del código de la interfaz de usuario.
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      24/04/2026 - Creacion (primera version) del codigo
**/

#include "menu_coordinator.h"
#include "algorithm_menu.h"
#include "instance_menu.h"
#include "local_search_menu.h"
#include "gvns_menu.h"
#include "console_utils.h"
#include "../model/run_config.h"
#include "../model/instance.h"
#include "../model/solution.h"
#include "../io/instance_reader.h"
#include "../constructive/greedy/greedy_constructive.h"
#include "../constructive/grasp/grasp_constructive.h"
#include "../algorithm/gvns/gvns.h"
#include "../local_search/strategies/vnd/simple_local_search.h"
#include "../local_search/strategies/rvnd/rvnd.h"
#include "../local_search/strategies/vnd_rl/vnd_rl.h"
#include "../local_search/strategies/vnd_rl/reward_strategy.h"
#include "../local_search/operators/shifts/shifts.h"
#include "../local_search/operators/swap_clients/swap_clients.h"
#include "../local_search/operators/swap_facilities/swap_facilities.h"
#include "../local_search/operators/incomp_elimination/incomp_elimination.h"
#include "../help/help_functions.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <limits>
#include <filesystem>

/**
 * @brief Asks the user for output file paths and updates the config accordingly. This includes:
 *        - The results file (where final results will be appended)
 *        - The directory for Q evolution logs (used if VND-RL is selected)
 *        - The configuration log file (where the parameters of each run will be recorded)
 * @param config The RunConfig object to update with the user's input.
 */
static void ui_askOutputPaths(RunConfig& config) {
  std::cout << "\n=== CONFIGURACIÓN DE SALIDA ===\n";
  config.output_results_file = ui_askFilePath(
      "Fichero de resultados [" + config.output_results_file + "]: ",
      config.output_results_file);
  config.output_q_dir = ui_askFilePath(
      "Directorio para evolución de Q (VND-RL) [" + config.output_q_dir + "]: ",
      config.output_q_dir);
  config.output_config_log = ui_askFilePath(
      "Fichero de registro de configuración [" + config.output_config_log + "]: ",
      config.output_config_log);
}

/**
 * @brief Builds the list of local search operators based on the user's configuration.
 * @param config The RunConfig containing the user's choices for operators.
 * @return A vector of shared pointers to the selected LocalSearch operators.
 */
static std::vector<std::shared_ptr<LocalSearch>> buildOperators(const RunConfig& config) {
  std::vector<std::shared_ptr<LocalSearch>> ops;
  if (config.use_operators.size() >= 4) {
    if (config.use_operators[0]) ops.push_back(std::make_shared<Shifts>());
    if (config.use_operators[1]) ops.push_back(std::make_shared<SwapClients>());
    if (config.use_operators[2]) ops.push_back(std::make_shared<SwapFacilities>());
    if (config.use_operators[3]) ops.push_back(std::make_shared<IncompElimination>());
  }
  return ops;
}

/**
 * @brief Creates the local search strategy object based on the user's configuration. This can be a simple VND, RVND, or VND with Reinforcement Learning (VND-RL).
 * @param config The RunConfig containing the user's choices for local search strategy and parameters.
 * @param ops The list of local search operators to use in the strategy.
 * @param instanceName The name of the instance being solved (used for logging in VND-RL).
 * @return A shared pointer to the created LocalSearch strategy object.
 */
static std::shared_ptr<LocalSearch> createLocalSearchStrategy(const RunConfig& config, const std::vector<std::shared_ptr<LocalSearch>>& ops, const std::string& instanceName) {
  std::vector<int> globalToLocal(4, -1);
  for (int i = 0; i < 4; ++i) {
    if (config.use_operators[i]) {
      int pos = 0;
      for (int j = 0; j < i; ++j) if (config.use_operators[j]) ++pos;
      globalToLocal[i] = pos;
    }
  }

  // Convert orders
  std::vector<int> localOrder;
  for (int globalIdx : config.rvnd_order) {
    int local = globalToLocal[globalIdx];
    if (local != -1) localOrder.push_back(local);
  }

  if (config.use_rl) {
    std::shared_ptr<RewardStrategy> rew;
    if (config.rl_reward_type == 2) {
        rew = std::make_shared<ProportionalReward>();
    } else if (config.rl_reward_type == 3) {
        rew = std::make_shared<NormalizedReward>(config.rl_max_delta);
    } else {
        rew = std::make_shared<BinaryReward>();
    }
    return std::make_shared<VND_RL>(ops, config.gvns_alpha_rl, config.gvns_epsilon, instanceName, rew,
                                    config.rl_max_sin_mejora, config.rl_max_total_iter, config.rl_decay,
                                    config.output_q_dir);   
  } else if (config.use_rvnd) {
    auto rvnd = std::make_shared<RVND>(ops);
    if (!config.rvnd_order.empty()) rvnd->setOrder(config.rvnd_order);
    return rvnd;
  } else {
    auto sls = std::make_shared<SimpleLocalSearch>(ops);
    if (!config.rvnd_order.empty()) sls->setOrder(config.rvnd_order);
    return sls;
  }
}

/**
 * @brief Applies the given local search strategy to the solution and checks feasibility after improvement.
 * @param solution The solution to improve.
 * @param inst The instance being solved (used for feasibility checks).
 * @param strategy The local search strategy to apply.
 */
static void applyLocalSearch(Solution& solution, const Instance& inst, std::shared_ptr<LocalSearch> strategy) {
  strategy->improve(solution, inst);
  solution.checkFeasibilityAfterLS(inst, "applyLocalSearch");
}

/**
 * @brief Ensures that the parent directory of a given file path exists, creating it if necessary. This is used to avoid errors when trying to write to a file in a non-existent directory.
 * @param filePath The file path for which to ensure the parent directory exists.
 */
static void ensureParentDirectoryExists(const std::string& filePath) {
  std::filesystem::path parent = std::filesystem::path(filePath).parent_path();
  if (!parent.empty() && !std::filesystem::exists(parent)) {
    std::filesystem::create_directories(parent);
  }
}

/**
 * @brief Writes a line of text to a specified file, ensuring that the parent directory exists. This is used for logging results and configurations.
 * @param filename The path of the file to write to.
 * @param line The line of text to write to the file.
 */
static void writeResultsToFile(const std::string& filename, const std::string& line) {
  ensureParentDirectoryExists(filename);
  std::ofstream out(filename, std::ios::app);
  if (out) {
    out << line << "\n";
  }
}

/**
 * @brief Dumps the current run configuration to the configuration log file. This includes all relevant parameters and settings chosen by the user, along with a timestamp. 
 *        This allows for later analysis of what configurations led to which results.
 * @param config The RunConfig object containing the current configuration to log.
 */
static void dumpConfigToFile(const RunConfig& config) {
  ensureParentDirectoryExists(config.output_config_log);  
  std::ofstream out(config.output_config_log, std::ios::app);
  if (!out) return;

  // Timestamp
  auto t = std::time(nullptr);
  out << "\n========================================\n";
  out << "Ejecución: " << std::ctime(&t); 
  out << "Algoritmos activos: "
      << (config.run_greedy ? "Voraz " : "")
      << (config.run_GRASP  ? "GRASP " : "")
      << (config.run_GVNS   ? "GVNS " : "") << "\n";

  if (config.run_GRASP) {
    out << "GRASP alpha=" << config.grasp_alpha
        << " iter=" << config.grasp_iters
        << " LS mode=";
    if (config.grasp_ls_mode == LSMode::NONE) out << "NONE";
    else if (config.grasp_ls_mode == LSMode::ONLY_LS) out << "ONLY_LS";
    else out << "BOTH";
    out << "\n";
  }
  if (config.run_greedy) {
    out << "Voraz LS mode=";
    if (config.greedy_ls_mode == LSMode::NONE) out << "NONE";
    else if (config.greedy_ls_mode == LSMode::ONLY_LS) out << "ONLY_LS";
    else out << "BOTH";
    out << "\n";
  }

  // Local strategy
  out << "Estrategia local: ";
  if (config.use_rl) {
    out << "VND-RL (alpha=" << config.gvns_alpha_rl
        << " epsilon=" << config.gvns_epsilon
        << " reward=" << (config.rl_reward_type == 1 ? "Binaria" : "Proporcional")
        << " decay=" << (config.rl_decay ? "Sí" : "No")
        << " maxSinMejora=" << config.rl_max_sin_mejora
        << " maxTotalIter=" << config.rl_max_total_iter << ")";
  } else if (config.use_rvnd) {
    out << "RVND";
  } else {
    out << "VND";
  }
  out << "\nOperadores seleccionados:";
  for (int i = 0; i < 4 && i < (int)config.use_operators.size(); ++i)
      if (config.use_operators[i]) out << " " << (i+1);
  out << "\nOrden fijo:";
  for (int idx : config.rvnd_order) out << " " << (idx+1);
  out << "\n";

  if (config.run_GVNS) {
    out << "GVNS kmax=" << config.gvns_kmax
        << " iter=" << config.gvns_iter
        << "\n";
  }

  out << "Instancias:";
  for (int idx : config.instance_indexes) out << " " << (idx+1);
  out << "\n";
  out << "Fichero resultados: " << config.output_results_file << "\n";
  out << "Directorio Q: " << config.output_q_dir << "\n";
  out << "========================================\n\n";
}

/**
 * @brief The main function that coordinates the user interface and the execution of algorithms. 
 *        It guides the user through configuring the run, selecting instances, 
 *        and then executes the selected algorithms on the chosen instances while logging results and configurations.
 */
void ui_launchApplication() {
  RunConfig config;

  // Ask output paths first to ensure logs can be saved even if the user cancels later
  ui_askOutputPaths(config);
  // Configure algorithms to run
  ui_configureAlgorithms(config);
  // Local search configuration (only ask if needed based on selected algorithms)
  bool needLSConfig = false;
  if (config.run_greedy) {
    config.greedy_ls_mode = ui_askLSMode("Voraz");
    if (config.greedy_ls_mode != LSMode::NONE) needLSConfig = true;
  }
  if (config.run_GRASP) {
    config.grasp_ls_mode = ui_askLSMode("GRASP");
    if (config.grasp_ls_mode != LSMode::NONE) needLSConfig = true;
  }
  if (config.run_GVNS) {
    needLSConfig = true;
  }
  if (needLSConfig) {
    ui_configureLocalSearch(config);
  }
  if (config.run_GVNS) {
    ui_configureGVNS(config);
  }
  // Save the configuration to the log file before running any algorithms
  dumpConfigToFile(config);

  // Select instances to run on
  auto instancePaths = ui_selectInstances(config);

  std::cout << "\n=== EJECUTANDO " << config.instance_indexes.size() << " INSTANCIA(S) ===\n";

  for (int idx : config.instance_indexes) {
    std::string path = instancePaths[idx];
    try {
        Instance inst = InstanceReader::read(path);
        std::string instName = std::filesystem::path(path).filename().string();
        std::cout << "\n" << std::string(60, '-') << "\n";
        std::cout << "Instancia: " << instName << "\n";
        printTableHeader();

        auto ops = buildOperators(config);

        // Greedy 
        if (config.run_greedy) {
          GreedyConstructive greedy;
          Timer t;
          Solution solution = greedy.build(inst);
          double timeNoLS = t.elapsedSeconds();

          if (config.greedy_ls_mode == LSMode::NONE || config.greedy_ls_mode == LSMode::BOTH) {
            std::string label = instName + " [voraz]";
            printSolutionRow(label, solution, inst, timeNoLS);
            writeResultsToFile(config.output_results_file,
                                label + "," + std::to_string(solution.getTotalCost()) + "," + std::to_string(timeNoLS));
          }

          if (config.greedy_ls_mode == LSMode::ONLY_LS || config.greedy_ls_mode == LSMode::BOTH) {
            t.reset();
            auto strategy = createLocalSearchStrategy(config, ops, instName);
            applyLocalSearch(solution, inst, strategy);
            std::string label = instName + " [voraz+LS]";
            printSolutionRow(label, solution, inst, t.elapsedSeconds());
            writeResultsToFile(config.output_results_file,
                                label + "," + std::to_string(solution.getTotalCost()) + "," + std::to_string(t.elapsedSeconds()));
          }
        }

        // GRASP
        if (config.run_GRASP) {
          GRASPConstructive grasp(config.grasp_alpha);
          if (config.grasp_ls_mode == LSMode::NONE || config.grasp_ls_mode == LSMode::BOTH) {
            Timer t;
            Solution best;
            double bestCost = std::numeric_limits<double>::max();
            for (int it = 0; it < config.grasp_iters; ++it) {
              grasp.reseed();
              Solution solution = grasp.build(inst);
              if (solution.getTotalCost() < bestCost) {
                bestCost = solution.getTotalCost();
                best = solution;
              }
            }
            std::string label = instName + " [GRASP α=" + std::to_string(config.grasp_alpha)
                                + " i=" + std::to_string(config.grasp_iters) + "]";
            printSolutionRow(label, best, inst, t.elapsedSeconds());
            writeResultsToFile(config.output_results_file,
                                label + "," + std::to_string(best.getTotalCost()) + "," + std::to_string(t.elapsedSeconds()));
          }

          if (config.grasp_ls_mode == LSMode::ONLY_LS || config.grasp_ls_mode == LSMode::BOTH) {
            Timer t;
            Solution best;
            double bestCost = std::numeric_limits<double>::max();
            for (int it = 0; it < config.grasp_iters; ++it) {
              grasp.reseed();
              Solution solution = grasp.build(inst);
              auto strategy = createLocalSearchStrategy(config, ops, instName);
              applyLocalSearch(solution, inst, strategy);
              if (solution.getTotalCost() < bestCost) {
                bestCost = solution.getTotalCost();
                best = solution;
              }
            }
            std::string label = instName + " [GRASP α=" + std::to_string(config.grasp_alpha)
                                + " i=" + std::to_string(config.grasp_iters) + " +LS]";
            printSolutionRow(label, best, inst, t.elapsedSeconds());
            writeResultsToFile(config.output_results_file,
                                label + "," + std::to_string(best.getTotalCost()) + "," + std::to_string(t.elapsedSeconds()));
          }
        }

        // GVNS
        if (config.run_GVNS) {
          Timer t;
          Solution best;
          double bestCost = std::numeric_limits<double>::max();
          auto strategy = createLocalSearchStrategy(config, ops, instName);
          for (int it = 0; it < config.gvns_iter; ++it) {
            GreedyConstructive greedy;
            Solution solution = greedy.build(inst);
            GVNS gvns(std::make_shared<GreedyConstructive>(), strategy, config.gvns_kmax, 10);
            gvns.setSeed(it + 12345);
            solution = gvns.run(inst);
            if (solution.getTotalCost() < bestCost) {
              bestCost = solution.getTotalCost();
              best = solution;
            }
          }
          std::string rlTag = config.use_rl ? " RL" : "";
          std::string label = instName + " [GVNS kmax=" + std::to_string(config.gvns_kmax)
                              + " i=" + std::to_string(config.gvns_iter) + rlTag + "]";
          printSolutionRow(label, best, inst, t.elapsedSeconds());
          writeResultsToFile(config.output_results_file,
                              label + "," + std::to_string(best.getTotalCost()) + "," + std::to_string(t.elapsedSeconds()));
        }
    } catch (const std::exception& e) {
      std::cerr << "[Error en " << std::filesystem::path(path).filename().string() << "] " << e.what() << "\n";
    }
  }
  std::cout << "\nResultados guardados en '" << config.output_results_file << "'.\n";
}