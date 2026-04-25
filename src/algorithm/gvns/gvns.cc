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

** Archivo gvns.cc: Implementación de la clase GVNS, que implementa el algoritmo General Variable Neighborhood Search para MS-CFLP-CI
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      24/04/2026 - Creacion (primera version) del codigo
**/

#include "gvns.h"
#include <algorithm>
#include <limits>

/**
 * @brief Constructor of the GVNS class. Initializes the GVNS algorithm with a given constructive method, local search improvement strategy, maximum neighborhood level (kmax), 
 *        and maximum number of iterations (maxIter). It also initializes the random number generator.
 * @param constructive A shared pointer to a Constructive object that will be used to build the initial solution for the GVNS algorithm.
 * @param improvement A shared pointer to a LocalSearch object that will be used to improve solutions during the GVNS algorithm.
 * @param kmax The maximum neighborhood level to be used in the shaking phase of the GVNS algorithm.
 * @param maxIter The maximum number of iterations to perform in the GVNS algorithm.
 */
GVNS::GVNS(std::shared_ptr<Constructive> constructive, std::shared_ptr<LocalSearch> improvement, double kmax, int maxIter)
          : constructive_(constructive), improvement_(improvement), kmax_(kmax), maxIter_(maxIter), rng_(std::random_device{}()) {}

/**
 * @brief Executes the GVNS algorithm on a given instance. It starts by building an initial solution using the constructive method, then iteratively applies 
 *        shaking and local search improvement until a stopping criterion is met (maximum iterations or no improvement for a certain number of iterations).
 * @param inst The instance of the problem to solve.
 * @return The best solution found by the GVNS algorithm for the given instance.
 */
Solution GVNS::run(const Instance& inst) {
  double actual_kmax = kmax_;
  if (use_percent_kmax_) {
    actual_kmax = std::max(1.0, (double)(percent_kmax_ / 100.0 * inst.stores));
  }

  Solution best = constructive_->build(inst);
  improvement_->improve(best, inst);
  double bestCost = best.getTotalCost();

  int iterWithoutImprove = 0;
  for (int it = 0; it < maxIter_ && iterWithoutImprove < 50; ++it) {
    int k = 1;
    while (k <= (int)actual_kmax) {
      Solution candidate = best;
      shake(candidate, k, inst);
      improvement_->improve(candidate, inst);
      if (candidate.isFeasible(inst) && candidate.getTotalCost() < bestCost - 1e-6) {
        best = candidate;
        bestCost = candidate.getTotalCost();
        k = 1;
        iterWithoutImprove = 0;
      } else {
        ++k;
      }
    }
    ++iterWithoutImprove;
  }
  return best;
}

/**
 * @brief Performs the shaking phase of the GVNS algorithm by randomly selecting k clients and reassigning them to different open and compatible facilities. 
 *        This perturbation is intended to escape local optima and explore new regions of the solution space.
 * @param solution The current solution to be perturbed.
 * @param k The number of clients to be randomly selected and reassigned.
 * @param inst The instance of the problem, which provides the necessary data for checking compatibility and capacity constraints during the reassignment.
 */
void GVNS::shake(Solution& solution, int k, const Instance& inst) {
  // Select k random clients that are currently assigned to some facility and try to reassign them to a different compatible facility
  std::vector<int> clientIndices(inst.stores);
  std::iota(clientIndices.begin(), clientIndices.end(), 0);
  std::shuffle(clientIndices.begin(), clientIndices.end(), rng_);

  int moved = 0;
  for (int idx = 0; idx < inst.stores && moved < k; ++idx) {
    int i = clientIndices[idx];
    if (solution.facilities_of[i].empty()) continue;
    int j1 = solution.facilities_of[i][std::uniform_int_distribution<int>(0, (int)solution.facilities_of[i].size()-1)(rng_)];
    double amount = solution.demand_fraction[i][j1] * inst.demand[i];
    if (amount < 1e-6) continue;

    // Choose a random open and compatible facility to reassign this client to (excluding the current one)
    std::vector<int> candidates;
    for (int j2 = 0; j2 < inst.warehouses; ++j2) {
      if (j2 == j1 || !solution.open[j2]) continue;
      if (solution.residual_cap[j2] < amount - 1e-6) continue;
      if (solution.incomp_count[i][j2] > 0) continue;  // compatible
      candidates.push_back(j2);
    }
    if (candidates.empty()) continue;

    int j2 = candidates[std::uniform_int_distribution<int>(0, (int)candidates.size()-1)(rng_)];
    solution.removeAssignment(i, j1, amount, inst);
    solution.assign(i, j2, amount, inst);
    ++moved;
  }
}