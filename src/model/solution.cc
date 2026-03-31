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

// ── Fábrica ───────────────────────────────────────────────────────────────────

Solution Solution::createEmpty(const Instance& inst) {
  Solution sol;
  const int m = inst.m, n = inst.n;

  sol.open.assign(m, false);
  sol.x.assign(n, std::vector<double>(m, 0.0));
  sol.w.assign(n, std::vector<bool>(m, false));
  sol.residualCap.assign(m, 0.0);
  sol.clientsOf.assign(m, {});
  sol.facilitiesOf.assign(n, {});
  sol.incompCount.assign(n, std::vector<int>(m, 0));
  sol.fixedCost_     = 0.0;
  sol.transportCost_ = 0.0;
  sol.totalCost_     = 0.0;
  return sol;
}

// ── Operaciones sobre instalaciones ──────────────────────────────────────────

void Solution::openFacility(int j, const Instance& inst) {
  if (!open[j]) {
    open[j]         = true;
    residualCap[j]  = inst.capacity[j];
    fixedCost_     += inst.fixedCost[j];
    totalCost_      = fixedCost_ + transportCost_;
  }
}

void Solution::closeFacility(int j, const Instance& inst) {
  if (open[j] && clientsOf[j].empty()) {
    open[j]         = false;
    residualCap[j]  = 0.0;
    fixedCost_     -= inst.fixedCost[j];
    totalCost_      = fixedCost_ + transportCost_;
  }
}

// ── Operaciones de asignación ─────────────────────────────────────────────────

void Solution::assign(int i, int j, double amount, const Instance& inst) {
  if (!open[j]) openFacility(j, inst);

  const bool wasServed = w[i][j];
  x[i][j]          += amount / inst.demand[i];
  residualCap[j]   -= amount;
  transportCost_   += inst.supplyCost[i][j] * amount;
  totalCost_        = fixedCost_ + transportCost_;

  if (!wasServed) {
    // Primera asignación de i a j: registrar en estructuras auxiliares
    w[i][j] = true;
    clientsOf[j].push_back(i);
    facilitiesOf[i].push_back(j);
    for (int ip : inst.incompNeighbors[i])
      incompCount[ip][j]++;
  }
}

void Solution::removeAssignment(int i, int j, double amount, const Instance& inst) {
  x[i][j]         -= amount / inst.demand[i];
  residualCap[j]  += amount;
  transportCost_  -= inst.supplyCost[i][j] * amount;
  totalCost_       = fixedCost_ + transportCost_;

  if (x[i][j] < EPS) {
    // i ya no está en j: limpiar estructuras auxiliares
    x[i][j] = 0.0;
    w[i][j] = false;

    auto& co = clientsOf[j];
    co.erase(std::remove(co.begin(), co.end(), i), co.end());

    auto& fo = facilitiesOf[i];
    fo.erase(std::remove(fo.begin(), fo.end(), j), fo.end());

    for (int ip : inst.incompNeighbors[i])
      incompCount[ip][j]--;
  }
}

// ── Validación ────────────────────────────────────────────────────────────────

bool Solution::isFeasible(const Instance& inst) const {
  return countIncompatibilityViolations(inst) == 0
      && computeUnsatisfiedDemand(inst) < EPS;
}

int Solution::countIncompatibilityViolations(const Instance& inst) const {
  int violations = 0;
  for (auto& [i1, i2] : inst.incompatiblePairs)
    for (int j = 0; j < inst.m; ++j)
      if (w[i1][j] && w[i2][j]) ++violations;
  return violations;
}

double Solution::computeUnsatisfiedDemand(const Instance& inst) const {
  double total = 0.0;
  for (int i = 0; i < inst.n; ++i) {
    double fraction = 0.0;
    for (int j = 0; j < inst.m; ++j) fraction += x[i][j];
    const double unsatisfied = 1.0 - fraction;
    if (unsatisfied > EPS) total += unsatisfied * inst.demand[i];
  }
  return total;
}

int Solution::openFacilitiesCount() const {
  return std::count(open.begin(), open.end(), true);
}

// ── Comprobación de factibilidad tras búsqueda local ─────────────────────────

#ifndef NDEBUG
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
#endif