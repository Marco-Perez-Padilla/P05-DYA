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

** Archivo swap_facilities.cc: Operador de búsqueda local para intercambiar instalaciones
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      31/03/2026 - Creacion (primera version) del codigo
**/

#include "./swap_facilities.h"
#include <limits>
#include <tuple>
#include <vector>

/**
 * @brief Implementation of the SwapFacilities local search operator. For each pair of (open facility to close, closed facility to open), it simulates closing the open facility and opening the closed one, redistributing the clients of the closed facility to the cheapest compatible open facility (including the newly opened one). If this results in a cost improvement, it applies the change.
 * @param solution The current solution to be improved.
 * @param inst The instance providing the problem data.
 * @return true if an improvement was found and applied, false otherwise.
 */
bool SwapFacilities::improve(Solution& solution, const Instance& inst) {
  bool any_improved = false;

  bool found_move = true;
  while (found_move) {
    found_move = false;

    // Iterate over all open facilities to consider closing
    for (int jclose = 0; jclose < inst.warehouses && !found_move; ++jclose) {
      if (!solution.open[jclose]) continue;
      if (solution.clients_of[jclose].empty()) {
        const double delta = -inst.fixed_cost[jclose];
        if (delta < -EPS) {
          solution.closeFacility(jclose, inst);
          found_move   = true;
          any_improved = true;
        }
        continue;
      }

      const std::vector<int> clients = solution.clients_of[jclose];
      for (int jnew = -1; jnew < inst.warehouses && !found_move; ++jnew) {
        if (jnew >= 0 && solution.open[jnew]) continue; 
        if (jnew == jclose) continue;

        std::vector<double> sim_residual(inst.warehouses);
        for (int j = 0; j < inst.warehouses; ++j)
            sim_residual[j] = solution.residual_cap[j];
        if (jnew >= 0)
            sim_residual[jnew] = inst.capacity[jnew]; 

        double transport_delta = 0.0;
        bool   feasible       = true;

        std::vector<std::tuple<int,int,double>> plan;
        plan.reserve(clients.size());

        for (int i : clients) {
          const double q = solution.demand_fraction[i][jclose] * inst.demand[i];

          int best_j    = -1;
          double best_cost = std::numeric_limits<double>::max();

          if (jnew >= 0 && sim_residual[jnew] >= q - EPS) {
              best_j    = jnew;
              best_cost = inst.supply_cost[i][jnew];
          }

          for (int j = 0; j < inst.warehouses; ++j) {
            if (j == jclose || j == jnew) continue;
            if (!solution.open[j]) continue;
            if (!solution.partial_attend[i][j] && solution.incomp_count[i][j] > 0) continue;
            if (sim_residual[j] < q - EPS) continue;
            if (inst.supply_cost[i][j] < best_cost) {
              best_cost = inst.supply_cost[i][j];
              best_j    = j;
            }
          }

          if (best_j < 0) { feasible = false; break; }

          sim_residual[best_j] -= q;
          transport_delta += (inst.supply_cost[i][best_j] - inst.supply_cost[i][jclose]) * q;
          plan.emplace_back(i, best_j, q);
        }

        if (!feasible) continue;

        const double fixed_delta = (jnew >= 0 ? inst.fixed_cost[jnew] : 0.0) - inst.fixed_cost[jclose];
        const double total_delta = fixed_delta + transport_delta;

        // If the total cost change is an improvement, apply the plan: 
        // 1. Close jclose and open jnew (if applicable), warehouse_2. Reassign clients according to the plan, 3. Update the solution's cost and feasibility.
        if (total_delta < -EPS) {
          for (auto& [i, dest, q] : plan) {
            solution.removeAssignment(i, jclose, q, inst);
          }
          solution.closeFacility(jclose, inst);

          if (jnew >= 0) solution.openFacility(jnew, inst);

          for (auto& [i, dest, q] : plan) {
              solution.assign(i, dest, q, inst);
          }
          
          found_move   = true;
          any_improved = true;
        }
      }
    }
  }

  return any_improved;
}