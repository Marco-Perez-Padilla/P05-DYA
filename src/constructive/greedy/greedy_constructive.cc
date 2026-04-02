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

** Archivo greedy_constructive.cc: Implementación: Clase GreedyConstructive para el MS-CFLP-CI
**/

#include "greedy_constructive.h"
#include <numeric>
#include <algorithm>

/**
 * @brief Constructive algorithm for MS-CFLP-CI 
 * @param inst The instance of the problem.
 * @return The constructed solution.
 *
 */
Solution GreedyConstructive::build(const Instance& inst) {
  Solution solution = Solution::createEmpty(inst);
  const int warehouses = inst.warehouses, stores = inst.stores;

  // Warehouses selection: open facilities in ascending order of fixed cost until demand is covered
  std::vector<int> order(warehouses);
  std::iota(order.begin(), order.end(), 0);
  std::sort(order.begin(), order.end(),
            [&](int a, int b){ return inst.fixed_cost[a] < inst.fixed_cost[b]; });

  double total_demand = 0.0;
  for (double dmd : inst.demand) {
    total_demand += dmd;
  }

  double acc_cap = 0.0;
  int index = 0;

  // Open warehouses until demand is covered
  while (index < warehouses && acc_cap < total_demand) {
    const int j = order[index++];
    solution.openFacility(j, inst);
    acc_cap += inst.capacity[j];
  }

  // Add extra facilities as slack for incompatibilities
  for (int extra = 0; extra < holgura_ && index < warehouses; ++extra, ++index) {
    solution.openFacility(order[index], inst);
  }

  // Clients assignation: for each client, assign to open facilities in ascending order of transport cost, checking capacity and incompatibilities.
  for (int i = 0; i < stores; ++i) {
    std::vector<int> fac_order;
    fac_order.reserve(warehouses);
    for (int j = 0; j < warehouses; ++j) {
        if (solution.open[j]) fac_order.push_back(j);
    }
    std::sort(fac_order.begin(), fac_order.end(),
              [&](int a, int b){ return inst.supply_cost[i][a] < inst.supply_cost[i][b]; });

    double remaining = inst.demand[i];
    for (int j : fac_order) {
      if (remaining < EPS) break;

      // Verify incompatibilities: if facility j is incompatible with any already assigned facility for client i, skip it
      if (solution.incomp_count[i][j] > 0) continue;

      // Verify residual capacity: if facility j has no residual capacity, skip it
      if (solution.residual_cap[j] < EPS) continue;

      const double q = std::min(remaining, solution.residual_cap[j]);
      solution.assign(i, j, q, inst);
      remaining -= q;
    }
  }
  return solution;
}