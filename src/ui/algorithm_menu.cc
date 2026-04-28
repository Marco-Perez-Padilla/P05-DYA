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

** Archivo algorithm_menu.cc: Implementación de las funciones para la configuración de algoritmos
**                              Resultado de la refactorización para mejorar la organización del código de la interfaz de usuario.
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      24/04/2026 - Creacion (primera version) del codigo
**/

#include "algorithm_menu.h"
#include "console_utils.h"
#include <iostream>

/**
 * @brief Asks the user to select which algorithms to execute (Greedy, GRASP, GVNS). The user can choose any combination of these algorithms. If GRASP is selected, it also asks for specific parameters related to GRASP (alpha and iterations).
 *        This function updates the provided RunConfig object with the user's choices, which will later be used to determine which algorithms to run and with what parameters.
 * @param config The RunConfig object to update with the user's algorithm configuration choices.
 */
void ui_configureAlgorithms(RunConfig& config) {
  std::cout << "\n=== MENÚ DE ALGORITMOS ===\n";
  std::cout << "Puede elegir cualquier combinación.\n";
  config.run_greedy = ui_askYesNo("¿Ejecutar Voraz?", true);
  config.run_GRASP  = ui_askYesNo("¿Ejecutar GRASP?", true);
  config.run_GVNS   = ui_askYesNo("¿Ejecutar GVNS?",   false);
  config.run_modified_gvns = ui_askYesNo("¿Ejecutar GVNS modificado?", true);
  if (config.run_modified_gvns) config.use_rvnd = true;
  if (config.run_GRASP) {
    std::cout << "\n--- Parámetros GRASP ---\n";
    config.grasp_alpha = ui_askInt("  Tamaño LRC (alpha) [3]: ", 3);
    config.grasp_iters = ui_askInt("  Iteraciones [100]: ", 100);
  }
}