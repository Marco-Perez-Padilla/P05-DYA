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

** Archivo swap_clients.h: Implementacion: Operador de búsqueda local para intercambiar clientes
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      31/03/2026 - Creacion (primera version) del codigo
**/

#include "./swap_clients.h"
#include <limits>

/**
 * @brief Implementation of the SwapClients local search operator. For each pair of clients client_1 and client_2 that are both served entirely by a single facility (|facilities_of[i]| == 1), 
 * it attempts to swap their assigned facilities. It checks capacity and incompatibility constraints for the swap, and if it results in a cost improvement, it applies the change.
 * @param solution The current solution to be improved.
 * @param inst The instance providing the problem data.
 * @return true if an improvement was found and applied, false otherwise.
 */
bool SwapClients::improve(Solution& solution, const Instance& inst) {
  bool any_improved = false;

  bool found_move = true;
  while (found_move) {
    found_move = false;

    double best_delta = -EPS;
    int best_client_1 = -1, best_client_2 = -1, best_warehouse_1 = -1, best_warehouse_2 = -1;

    for (int client_1 = 0; client_1 < inst.stores; ++client_1) {
      if ((int)solution.facilities_of[client_1].size() != 1) continue;
      const int warehouse_1 = solution.facilities_of[client_1][0];

      for (int client_2 = client_1 + 1; client_2 < inst.stores; ++client_2) {
        if ((int)solution.facilities_of[client_2].size() != 1) continue;
        const int warehouse_2 = solution.facilities_of[client_2][0];
        if (warehouse_1 == warehouse_2) continue; 

        const double demand_1 = inst.demand[client_1];
        const double demand_2 = inst.demand[client_2];

        if (solution.residual_cap[warehouse_1] + demand_1 < demand_2 - EPS) continue;
        if (solution.residual_cap[warehouse_2] + demand_2 < demand_1 - EPS) continue;

        int incomp_client1_in_warehouse2 = solution.incomp_count[client_1][warehouse_2];
        if (inst.is_incompat[client_1][client_2]) --incomp_client1_in_warehouse2;
        if (incomp_client1_in_warehouse2 > 0) continue;

        int incomp_client2_in_warehouse1 = solution.incomp_count[client_2][warehouse_1];
        if (inst.is_incompat[client_1][client_2]) --incomp_client2_in_warehouse1;
        if (incomp_client2_in_warehouse1 > 0) continue;

        const double delta =
            (inst.supply_cost[client_1][warehouse_2] - inst.supply_cost[client_1][warehouse_1]) * demand_1 +
            (inst.supply_cost[client_2][warehouse_1] - inst.supply_cost[client_2][warehouse_2]) * demand_2;

        if (delta < best_delta) {
          best_delta = delta;
          best_client_1 = client_1; best_client_2 = client_2;
          best_warehouse_1 = warehouse_1; best_warehouse_2 = warehouse_2;
        }
      }
    }

    if (best_client_1 >= 0) {
      const double demand_1 = inst.demand[best_client_1];
      const double demand_2 = inst.demand[best_client_2];
      solution.removeAssignment(best_client_1, best_warehouse_1, demand_1, inst);
      solution.removeAssignment(best_client_2, best_warehouse_2, demand_2, inst);
      solution.assign(best_client_1, best_warehouse_2, demand_1, inst);
      solution.assign(best_client_2, best_warehouse_1, demand_2, inst);
      found_move   = true;
      any_improved = true;
    }
  }

  return any_improved;
}