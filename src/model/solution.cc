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

** Archivo solution.cc: Implementación de la clase Solution
**/

#include "solution.h"
#include <algorithm>
#include <numeric>

/**
 * @brief Creates an empty solution with all data structures initialized but no facilities open and no demand assigned.
 * @param inst The instance for which to create the solution (used to initialize sizes and capacities).
 * @return A new Solution object representing an empty solution.
 */
Solution Solution::createEmpty(const Instance& inst) {
  Solution solution;
  const int warehouses = inst.warehouses, stores = inst.stores;

  solution.open.assign(warehouses, false);
  solution.demand_fraction.assign(stores, std::vector<double>(warehouses, 0.0));
  solution.partial_attend.assign(stores, std::vector<bool>(warehouses, false));
  solution.residual_cap.assign(warehouses, 0.0);
  solution.clients_of.assign(warehouses, {});
  solution.facilities_of.assign(stores, {});
  solution.incomp_count.assign(stores, std::vector<int>(warehouses, 0));
  solution.fixed_cost_     = 0.0;
  solution.trasnport_cost_ = 0.0;
  solution.total_cost_     = 0.0;
  return solution;
}

/**
 * @brief Opens facility j if it is not already open, updating costs and residual capacity.
 * @param j The index of the facility to open.
 * @param inst The instance providing the fixed cost and capacity of the facility.
 */
void Solution::openFacility(int j, const Instance& inst) {
  if (!open[j]) {
    open[j]         = true;
    residual_cap[j]  = inst.capacity[j];
    fixed_cost_     += inst.fixed_cost[j];
    total_cost_      = fixed_cost_ + trasnport_cost_;
  }
}

/**
 * @brief Closes facility j if it is open and has no clients assigned, updating costs and residual capacity.
 * @param j The index of the facility to close.
 * @param inst The instance providing the fixed cost of the facility.
 */
void Solution::closeFacility(int j, const Instance& inst) {
  if (open[j] && clients_of[j].empty()) {
    open[j]         = false;
    residual_cap[j]  = 0.0;
    fixed_cost_     -= inst.fixed_cost[j];
    total_cost_      = fixed_cost_ + trasnport_cost_;
  }
}

/**
 * @brief Assigns a certain amount of demand from client i to facility j, updating all relevant data structures and costs.
 * If the facility is not open, it will be opened automatically.
 * @param i The index of the client.
 * @param j The index of the facility.
 * @param amount The amount of demand to assign (must be positive and not exceed the client's demand or the facility's residual capacity).
 * @param inst The instance providing demand, supply cost,
 */
void Solution::assign(int i, int j, double amount, const Instance& inst) {
  if (!open[j]) openFacility(j, inst);

  const bool wasServed = partial_attend[i][j];
  demand_fraction[i][j]          += amount / inst.demand[i];
  residual_cap[j]   -= amount;
  trasnport_cost_   += inst.supply_cost[i][j] * amount;
  total_cost_        = fixed_cost_ + trasnport_cost_;

  if (!wasServed) {
    partial_attend[i][j] = true;
    clients_of[j].push_back(i);
    facilities_of[i].push_back(j);
    for (int ip : inst.incomp_neighbors[i])
      incomp_count[ip][j]++;
  }
}

/**
 * @brief Removes a certain amount of demand from the assignment of client i to facility j, updating all relevant data structures and costs.
 * If the resulting demand fraction for i at j drops to zero, i is removed from all auxiliary structures related to j.
 * @param i The index of the client.
 * @param j The index of the facility.
 * @param amount The amount of demand to remove (must be positive and not exceed the currently assigned amount).
 * @param inst The instance providing demand, supply cost, and incompatibility information.
 */
void Solution::removeAssignment(int i, int j, double amount, const Instance& inst) {
  demand_fraction[i][j]         -= amount / inst.demand[i];
  residual_cap[j]  += amount;
  trasnport_cost_  -= inst.supply_cost[i][j] * amount;
  total_cost_       = fixed_cost_ + trasnport_cost_;

  if (demand_fraction[i][j] < EPS) {
    // i ya no está en j: limpiar estructuras auxiliares
    demand_fraction[i][j] = 0.0;
    partial_attend[i][j] = false;

    auto& co = clients_of[j];
    co.erase(std::remove(co.begin(), co.end(), i), co.end());

    auto& fo = facilities_of[i];
    fo.erase(std::remove(fo.begin(), fo.end(), j), fo.end());

    for (int ip : inst.incomp_neighbors[i])
      incomp_count[ip][j]--;
  }
}

/**
 * @brief Checks if the solution is feasible by verifying that there are no incompatibility violations and that all demand is satisfied within a tolerance.
 * @param inst The instance providing incompatibility and demand information.
 * @return true if the solution is feasible, false otherwise.
 */
bool Solution::isFeasible(const Instance& inst) const {
  return countIncompatibilityViolations(inst) == 0
      && computeUnsatisfiedDemand(inst) < EPS;
}

/**
 * @brief Counts the number of incompatibility violations in the solution by checking all pairs of incompatible clients and their assigned facilities.
 * @param inst The instance providing the list of incompatible pairs and their assigned facilities.
 * @return The total number of incompatibility violations in the solution.
 */
int Solution::countIncompatibilityViolations(const Instance& inst) const {
  int violations = 0;
  for (auto& [i1, i2] : inst.incompatible_pairs)
    for (int j = 0; j < inst.warehouses; ++j)
      if (partial_attend[i1][j] && partial_attend[i2][j]) ++violations;
  return violations;
}

/**
 * @brief Computes the total unsatisfied demand in the solution by summing the unmet demand for each client based on their assigned demand fractions.
 * @param inst The instance providing demand information.
 * @return The total amount of unsatisfied demand in the solution.
 */
double Solution::computeUnsatisfiedDemand(const Instance& inst) const {
  double total = 0.0;
  for (int i = 0; i < inst.stores; ++i) {
    double fraction = 0.0;
    for (int j = 0; j < inst.warehouses; ++j) fraction += demand_fraction[i][j];
    const double unsatisfied = 1.0 - fraction;
    if (unsatisfied > EPS) total += unsatisfied * inst.demand[i];
  }
  return total;
}

/**
 * @brief Counts the number of open facilities in the solution by summing the boolean values in the open vector.
 * @return The total number of open facilities in the solution.
 */
int Solution::openFacilitiesCount() const {
  return std::count(open.begin(), open.end(), true);
}

/**
 * @brief Checks the feasibility of the solution after local search by verifying that there are no incompatibility violations and that all demand is satisfied within a tolerance.
 * If the solution is infeasible, an InfeasibleSolutionException is thrown with details about the violations and unsatisfied demand. 
 * @param inst The instance providing incompatibility and demand information.
 * @param context An optional string providing context about the local search operation that was performed, included in the exception message if the solution is infeasible.
 * @throws InfeasibleSolutionException if the solution is infeasible, with details about the number of incompatibility violations and
 */
void Solution::checkFeasibilityAfterLS(const Instance& inst,
                                        const std::string& context) const {
  if (!isFeasible(inst)) {
    const int    violations  = countIncompatibilityViolations(inst);
    const double unsatisfied = computeUnsatisfiedDemand(inst);
    const std::string prefix = context.empty() ? "" : "[" + context + "] ";
    throw InfeasibleSolutionException(
        prefix +
        "incompatibility violations=" + std::to_string(violations) +
        ", unsatisfied demand="       + std::to_string(unsatisfied));
  }
}
