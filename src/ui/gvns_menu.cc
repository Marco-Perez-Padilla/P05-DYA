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

** Archivo gvns_menu.cc: Implementación de las funciones que coordinan la configuración del GVNS
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      24/04/2026 - Creacion (primera version) del codigo
**/

#include "gvns_menu.h"
#include "console_utils.h"
#include <iostream>

/**
 * @brief Configures the GVNS algorithm parameters based on user input. This includes setting the maximum neighborhood level (kmax) and the maximum number of iterations for the GVNS.
 *       It also informs the user about the local search strategy that will be used during the GVNS improvement phase, which is determined by previous configurations (VND, RVND, or VND-RL).
 * @param config The RunConfig object to update with the user's GVNS configuration choices.
 */
void ui_configureGVNS(RunConfig& config) {
  std::cout << "\n=== CONFIGURACIÓN GVNS ===\n";
  config.gvns_kmax = ui_askInt("kmax (nivel máximo de perturbación) [3]: ", 3);
  config.gvns_iter = ui_askInt("Iteraciones máximas del GVNS [100]: ", 100);
  const char* strategy = config.use_rl ? "VND-RL" : (config.use_rvnd ? "RVND" : "VND");
  std::cout << "La mejora local usará la estrategia seleccionada previamente: " << strategy << ".\n";
}