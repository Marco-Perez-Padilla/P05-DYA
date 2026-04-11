/**
** Universidad de La Laguna
** Escuela Superior de Ingenieria y Tecnologia
** Grado en Ingenieria Informatica
** Asignatura: Diseño y Analisis de Algoritmos
** Curso: 3º
** Practica 5: Algoritmos constructivos y búsquedas por entornos
** Autor: Marco Pérez Padilla
** Correo: alu0101469348@ull.edu.es
** Fecha: 11/04/2026

** Archivo incomp_elimination.cc: Implementación del operador de búsqueda local para eliminar incompatibilidades
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      11/04/2026 - Creacion (primera version) del codigo
**/

#include "incomp_elimination.h"
#include <limits>
#include <algorithm>

/**
 * @brief Implementation of the IncompElimination local search operator. It identifies the open facility with the highest transportation cost per unit of capacity used, then finds the client that causes the most incompatibility violations in that facility. It attempts to move all of that client's demand to another open facility that is compatible and cheaper, if such a move results in a cost improvement.
 * @param solution The current solution to be improved.
 * @param inst The instance providing the problem data.
 * @return true if an improvement was found and applied, false otherwise.
 */
bool IncompElimination::improve(Solution& solution, const Instance& inst) {
  bool any_improved = false;

  bool found_move = true;
  while (found_move) {
    found_move = false;

    int worst_j = -1;
    double worst_ratio = -1.0;
    for (int j = 0; j < inst.warehouses; ++j) {
      if (!solution.open[j] || solution.clients_of[j].empty()) continue;
      double transport_j = 0.0;
      for (int i : solution.clients_of[j]) {
        transport_j += solution.demand_fraction[i][j] * inst.demand[i] * inst.supply_cost[i][j];
      }
      double ratio = (solution.residual_cap[j] > EPS) ? transport_j / (inst.capacity[j] - solution.residual_cap[j]) : transport_j;
      if (ratio > worst_ratio) {
        worst_ratio = ratio;
        worst_j = j;
      }
    }
    if (worst_j < 0) break;

    int best_i = -1;
    int max_incomp = 0;
    for (int i : solution.clients_of[worst_j]) {
      int incomp_here = solution.incomp_count[i][worst_j];
      if (incomp_here > max_incomp) {
        max_incomp = incomp_here;
        best_i = i;
      }
    }
    if (best_i < 0) break;

    const double demand_i = solution.demand_fraction[best_i][worst_j] * inst.demand[best_i];
    if (demand_i < EPS) break;

    double best_delta = -EPS;
    int best_dest = -1;
    for (int j2 = 0; j2 < inst.warehouses; ++j2) {
      if (!solution.open[j2] || j2 == worst_j) continue;
      if (solution.residual_cap[j2] < demand_i - EPS) continue;
      if (!solution.partial_attend[best_i][j2] && solution.incomp_count[best_i][j2] > 0) continue;
      double delta = (inst.supply_cost[best_i][j2] - inst.supply_cost[best_i][worst_j]) * demand_i;
      if (delta < best_delta) {
        best_delta = delta;
        best_dest = j2;
      }
    }

    if (best_dest >= 0) {
      solution.removeAssignment(best_i, worst_j, demand_i, inst);
      solution.assign(best_i, best_dest, demand_i, inst);
      found_move = true;
      any_improved = true;
    }
  }
  return any_improved;
}