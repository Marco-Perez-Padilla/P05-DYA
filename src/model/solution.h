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

** Archivo solution.h: Clase Solution para el MS-CFLP-CI
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      31/03/2026 - Creacion (primera version) del codigo
**      11/04/2026 - Modificación para añadir métodos de verificación de factibilidad y consistencia
**/

#ifndef SOLUTION_H
#define SOLUTION_H

#include <vector>
#include <string>
#include "instance.h"
#include "../exceptions/exceptions.h"

/// Tolerance
constexpr double EPS = 1e-9;

/**
 * @brief Representación de una solución para el MS-CFLP-CI.
 */
class Solution {
 private:
  double fixed_cost_     = 0.0;
  double trasnport_cost_ = 0.0;
  double total_cost_     = 0.0;

 public:
  std::vector<bool> open;  
  std::vector<std::vector<double>> demand_fraction;     
  std::vector<std::vector<bool>> partial_attend;

  std::vector<double> residual_cap;  
  std::vector<std::vector<int>> clients_of; 
  std::vector<std::vector<int>> facilities_of;
  std::vector<std::vector<int>> incomp_count;

  static Solution createEmpty(const Instance& inst);

  void openFacility(int j, const Instance& inst);
  void closeFacility(int j, const Instance& inst);

  void assign(int i, int j, double amount, const Instance& inst);
  void removeAssignment(int i, int j, double amount, const Instance& inst);

  double getFixedCost() const {return fixed_cost_;}
  double getTransportCost() const {return trasnport_cost_;}
  double getTotalCost() const {return total_cost_;}

  bool isFeasible(const Instance& inst) const;
  void checkConsistency(const Instance& inst) const;
  int countIncompatibilityViolations(const Instance& inst) const;
  double computeUnsatisfiedDemand(const Instance& inst) const;
  int openFacilitiesCount() const;

  void checkFeasibilityAfterLS(const Instance& inst, const std::string& context = "") const;
};

#endif