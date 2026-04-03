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

** Archivo shifts.h: Operador de búsqueda local para mover clientes entre instalaciones
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      31/03/2026 - Creacion (primera version) del codigo
**/

#include "./shifts.h"
#include <limits>
#include <algorithm>

/**
 * @brief Implementation of the Shifts local search operator. For each client i served by warehouse_1, it attempts to move as much of its demand as possible to a cheaper open facility warehouse_2, 
 * while respecting warehouse_2's residual capacity and incompatibility constraints. It uses a best improvement strategy, applying the move with the greatest cost reduction in each iteration until no improvement is found.
 * @param solution The current solution to be improved.
 * @param inst The instance providing the problem data.
 * @return true if an improvement was found and applied, false otherwise.
 */
bool Shifts::improve(Solution& solution, const Instance& inst) {
  bool any_improved = false;

  bool found_move = true;
  while (found_move) {
    found_move = false;

    double best_delta = -EPS;
    int    best_client = -1, best_warehouse_1 = -1, best_warehouse_2 = -1;
    double best_quantity = 0.0;

    for (int i = 0; i < inst.stores; ++i) {
      for (int warehouse_1 : solution.facilities_of[i]) {
        const double amount_at_warehouse1 = solution.demand_fraction[i][warehouse_1] * inst.demand[i];
        if (amount_at_warehouse1 < EPS) continue;

        for (int warehouse_2 = 0; warehouse_2 < inst.warehouses; ++warehouse_2) {
          if (!solution.open[warehouse_2] || warehouse_2 == warehouse_1) continue;
          if (inst.supply_cost[i][warehouse_2] >= inst.supply_cost[i][warehouse_1]) continue;
          if (!solution.partial_attend[i][warehouse_2] && solution.incomp_count[i][warehouse_2] > 0) continue;
          if (solution.residual_cap[warehouse_2] < EPS) continue;

          const double max_movable_quantity = std::min(amount_at_warehouse1, solution.residual_cap[warehouse_2]);
          if (max_movable_quantity < EPS) continue;

          const double delta = (inst.supply_cost[i][warehouse_2] - inst.supply_cost[i][warehouse_1]) * max_movable_quantity;

          if (delta < best_delta) {
            best_delta = delta;
            best_client = i; best_warehouse_1 = warehouse_1; best_warehouse_2 = warehouse_2;
            best_quantity = max_movable_quantity;
          }
        }
      }
    }

    if (best_client >= 0) {
      solution.removeAssignment(best_client, best_warehouse_1, best_quantity, inst);
      solution.assign(best_client, best_warehouse_2, best_quantity, inst);
      found_move    = true;
      any_improved  = true;
    }
  }

  return any_improved;
}