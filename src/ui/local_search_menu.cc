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

** Archivo local_search_menu.cc: Implementación de las funcionas que coordinan la configuración de la búsqueda local
**                              Resultado de la refactorización para mejorar la organización del código de la interfaz de usuario.
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      24/04/2026 - Creacion (primera version) del codigo
**/

#include "local_search_menu.h"
#include "console_utils.h"
#include <iostream>
#include <sstream>
#include <algorithm>

/**
 * @brief Asks the user to select the local search mode for a given algorithm (e.g., GVNS). The options include no local search, only local search, or both. 
 *        This allows the user to easily configure whether they want to apply local search after the constructive phase, and if so, whether they want to compare results with and without local search.
 * @param algorithmName The name of the algorithm for which the local search mode is being configured (used in prompts).
 * @return The selected LSMode based on the user's input.
 */
LSMode ui_askLSMode(const std::string& algorithmName) {
  std::cout << "\nModo de búsqueda local para " << algorithmName << ":\n"
            << "  [1] Sin búsqueda local (solo constructivo)\n"
            << "  [2] Con búsqueda local\n"
            << "  [3] Ambos (sin y con LS)\n";
  int op = ui_askOption("Opción [2]: ", 1, 3, 2);
  if (op == 1) return LSMode::NONE;
  if (op == 3) return LSMode::BOTH;
  return LSMode::ONLY_LS;
}

/**
 * @brief Parses a user input line to determine the order of local search operators, ensuring that the input is valid. 
 *        The user must provide a sequence of numbers corresponding to the selected operators, without duplicates and only including those that were chosen to be used.
 * @param line The input line from the user specifying the order of operators.
 * @param numSelected The number of operators that were selected to be used (used to validate the number of operators in the order).
 * @param useOps A vector of booleans indicating
 */
static std::vector<int> parseOrderLineStrict(const std::string& line, int numSelected, const std::vector<bool>& useOps) {
  std::vector<int> order;
  std::stringstream ss(line);
  int idx;
  while (ss >> idx) {
    if (idx < 1 || idx > 4) return {};            
    if (!useOps[idx - 1]) return {};           
    if (std::find(order.begin(), order.end(), idx - 1) != order.end()) {
      return {};                                
    }
    order.push_back(idx - 1);
  }
  if ((int)order.size() != numSelected) return {};  
  return order;
}

/**
 * @brief Configures the local search settings based on user input. This includes selecting the type of local search (VND, RVND, VND-RL) and which operators to include. 
 *        It also handles the order of operators if applicable. The configuration is stored in the provided RunConfig object
 * @param config The RunConfig object to update with the user's local search configuration choices.
 */
void ui_configureLocalSearch(RunConfig& config) {
  std::cout << "\n=== ESTRATEGIA DE BÚSQUEDA LOCAL ===\n"
            << "  [1] VND (orden fijo)\n"
            << "  [2] RVND (orden aleatorio en cada iteración)\n"
            << "  [3] VND con Reinforcement Learning (VND-RL)\n";
  int choice = ui_askOption("Opción [1]: ", 1, 3, 1);
  config.use_rvnd = false;
  config.use_rl   = false;

  if (choice == 3) {
    config.use_rl = true;
    std::cout << "\n--- Parámetros VND-RL ---\n";
    config.gvns_alpha_rl  = ui_askDouble("  Tasa de aprendizaje (alpha) [0.1]: ", 0.1);
    config.gvns_epsilon   = ui_askDouble("  Epsilon (exploración) [0.2]: ", 0.2);
    if (config.gvns_epsilon < 0.01) {
        std::cout << "  Epsilon demasiado pequeño, usando 0.2 por defecto.\n";
        config.gvns_epsilon = 0.2;
    }
    std::cout << "  Tipo de recompensa:\n"
              << "    [1] Binaria (0/1)\n"
              << "    [2] Proporcional (mejora relativa)\n"
              << "    [3] Normalizada (mejora relativa normalizada por la mejor mejora vista)\n";
    int rewType = ui_askOption("  Opción [1]: ", 1, 3, 1);
    if (rewType == 3) {
      config.rl_max_delta = ui_askDouble("    Máxima mejora esperada (maxDelta) [1.0]: ", 1.0);
    }
    config.rl_reward_type = rewType; 
    config.rl_decay = ui_askYesNo("  ¿Usar decaimiento exponencial de epsilon?", false);
    config.rl_max_sin_mejora = ui_askInt("  Máx. iteraciones sin mejora [5]: ", 5);
    config.rl_max_total_iter  = ui_askInt("  Máx. iteraciones totales [10]: ", 10);
  } else if (choice == 2) {
    config.use_rvnd = true;
  }

  const std::vector<std::string> opNames = {"Shift", "SwapClientes", "SwapInstalaciones", "EliminacionIncomp"};
  std::cout << "\nSeleccione los operadores (S/N):\n";
  config.use_operators.resize(4, true);
  for (size_t i = 0; i < 4; ++i) {
    config.use_operators[i] = ui_askYesNo("  Incluir " + opNames[i] + "?", true);
  }

  int numSelected = (int)std::count(config.use_operators.begin(), config.use_operators.end(), true);
  if (numSelected > 0) {
    std::cout << "\nOrden de vecindades (números 1-4):\n"
              << "  1:Shift  2:SwapClientes  3:SwapInstalaciones  4:EliminacionIncomp\n";
    if (config.use_rvnd) {
      std::cout << "RVND: orden inicial, luego se baraja aleatoriamente.\n";
    } else if (config.use_rl) {
      std::cout << "VND-RL: el orden lo determina la política Q (el orden fijo no se usa).\n";
    }

    std::vector<int> order;
    bool orderValid = false;
    do {
      std::cout << "Orden [";
      for (int i = 0; i < 4; ++i) {
        if (config.use_operators[i]) std::cout << " " << (i + 1);
      }
      std::cout << " ]: ";

      std::string line;
      std::getline(std::cin, line);
      order = parseOrderLineStrict(line, numSelected, config.use_operators);
      if (order.empty()) {
        std::cout << "  Entrada inválida. Debe contener exactamente los números de los operadores seleccionados, sin repetir.\n";
        std::cout << "  Operadores disponibles:";
        for (int i = 0; i < 4; ++i) {
          if (config.use_operators[i]) std::cout << " " << (i + 1);
        }
        std::cout << "\n";
      } else {
        orderValid = true;
      }
    } while (!orderValid);
    config.rvnd_order = std::move(order);
  } else {
    config.rvnd_order.clear();
  }
}