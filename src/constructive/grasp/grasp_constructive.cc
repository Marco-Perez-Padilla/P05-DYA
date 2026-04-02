/** Universidad de La Laguna
** Escuela Superior de Ingenieria y Tecnologia
** Grado en Ingenieria Informatica
** Asignatura: Diseño y Analisis de Algoritmos
** Curso: 3º
** Practica 5: Algoritmos constructivos y búsquedas por entornos
** Autor: Marco Pérez Padilla
** Correo: alu0101469348@ull.edu.es
** Fecha: 31/03/2026

** Archivo grasp_constructive.cc: Implementación: Clase GRASPConstructive para el MS-CFLP-CI
**/

#include "grasp_constructive.h"
#include <numeric>
#include <algorithm>

/**
 * @brief GRASP constructive algorithm for MS-CFLP-CI
 * @param inst The instance of the problem.
 * @return The constructed solution.
 */
Solution GRASPConstructive::build(const Instance& inst) {
  Solution solution = Solution::createEmpty(inst);
  const int warehouses = inst.warehouses, stores = inst.stores;

  // Warehouses randomized selection: open facilities in ascending order of fixed cost until demand is covered,
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

  // Open warehouses choosing randomly from the RCL until demand is covered
  while (index < warehouses && acc_cap < total_demand) {
    const int rcl_size = std::min(alpha_, warehouses - index);
    std::uniform_int_distribution<int> pick(0, rcl_size - 1);
    const int chosen = order[index + pick(rng_)];

    auto it = std::find(order.begin() + index,
                        order.begin() + index + rcl_size, chosen);
    std::iter_swap(it, order.begin() + index);

    solution.openFacility(chosen, inst);
    acc_cap += inst.capacity[chosen];
    ++index;
  }

  //Add extra facilities as slack for incompatibilities choosing randomly from the RCL
  for (int extra = 0; extra < holgura_ && index < warehouses; ++extra) {
    const int rcl_size = std::min(alpha_, warehouses - index);
    std::uniform_int_distribution<int> pick(0, rcl_size - 1);
    const int chosen = order[index + pick(rng_)];

    auto it = std::find(order.begin() + index,
                        order.begin() + index + rcl_size, chosen);
    std::iter_swap(it, order.begin() + index);

    solution.openFacility(chosen, inst);
    ++index;
  }

  // Clients randomized assignation: for each client, assign to open facilities in ascending order of transport cost, checking capacity and incompatibilities. 
  // The order of clients is also randomized.
  std::vector<int> client_order(stores);
  std::iota(client_order.begin(), client_order.end(), 0);
  std::shuffle(client_order.begin(), client_order.end(), rng_);

  for (int i : client_order) {
    std::vector<int> fac_order;
    fac_order.reserve(warehouses);
    for (int j = 0; j < warehouses; ++j) {
      if (solution.open[j]) fac_order.push_back(j);
    }
    std::sort(fac_order.begin(), fac_order.end(),
              [&](int a, int b){ return inst.supply_cost[i][a] < inst.supply_cost[i][b]; });

    double remaining = inst.demand[i];
    while (remaining > EPS) {
      std::vector<int> rcl;
      rcl.reserve(alpha_);
      for (int j : fac_order) {
          if (solution.incomp_count[i][j] > 0) continue;
          if (solution.residual_cap[j] < EPS)  continue;
          rcl.push_back(j);
          if ((int)rcl.size() >= alpha_) break;
      }
      if (rcl.empty()) break; 

      std::uniform_int_distribution<int> pick(0, (int)rcl.size() - 1);
      const int j = rcl[pick(rng_)];

      const double q = std::min(remaining, solution.residual_cap[j]);
      solution.assign(i, j, q, inst);
      remaining -= q;
    }
  }
  return solution;
}