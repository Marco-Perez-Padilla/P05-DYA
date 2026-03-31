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
**/

#pragma once
#include <vector>
#include <string>
#include "instance.h"
#include "../exceptions/exceptions.h"

/// Tolerancia para comparaciones de punto flotante
constexpr double EPS = 1e-9;

/**
 * @brief Representación de una solución para el MS-CFLP-CI.
 *
 * Contiene las estructuras primarias y auxiliares descritas en
 * la Tabla 10 de la práctica, más métodos atómicos de modificación
 * que mantienen la coherencia de todas las estructuras.
 *
 * Los campos de coste (fixedCost, transportCost, totalCost) son privados
 * y se mantienen de forma incremental mediante los métodos de modificación.
 * El resto de estructuras son públicas para lectura directa por los operadores.
 */
class Solution {
 public:
  // ── Estructuras primarias (lectura directa necesaria por los operadores) ──
  std::vector<bool>                open;  ///< open[j]=true si j está abierta
  std::vector<std::vector<double>> x;     ///< x[i][j]: fracción de d[i] servida por j
  std::vector<std::vector<bool>>   w;     ///< w[i][j]=true si i es atendido (parcialmente) por j

  // ── Estructuras auxiliares ────────────────────────────────────────────────
  std::vector<double>           residualCap;  ///< capacidad libre de j: s[j] - Σ d[i]·x[i][j]
  std::vector<std::vector<int>> clientsOf;    ///< clientes servidos por j
  std::vector<std::vector<int>> facilitiesOf; ///< instalaciones que sirven a i
  std::vector<std::vector<int>> incompCount;  ///< incompCount[i][j]: nº incompatibles de i en j

  // ── Fábrica ───────────────────────────────────────────────────────────────
  static Solution createEmpty(const Instance& inst);

  // ── Operaciones sobre instalaciones ──────────────────────────────────────
  void openFacility(int j, const Instance& inst);
  /// Cierra j solo si no tiene clientes asignados
  void closeFacility(int j, const Instance& inst);

  // ── Operaciones de asignación ─────────────────────────────────────────────
  /// Asigna `amount` unidades de demanda del cliente i a j.
  /// Actualiza todas las estructuras auxiliares y costes incrementalmente.
  void assign(int i, int j, double amount, const Instance& inst);

  /// Elimina `amount` unidades de la asignación de i en j.
  /// Si x[i][j] → 0, elimina i de todas las estructuras auxiliares de j.
  void removeAssignment(int i, int j, double amount, const Instance& inst);

  // ── Getters de coste (privados internamente, acceso de solo lectura) ──────
  double getFixedCost()     const { return fixedCost_;     }
  double getTransportCost() const { return transportCost_; }
  double getTotalCost()     const { return totalCost_;     }

  // ── Validación ────────────────────────────────────────────────────────────
  bool   isFeasible(const Instance& inst) const;
  int    countIncompatibilityViolations(const Instance& inst) const;
  double computeUnsatisfiedDemand(const Instance& inst) const;
  int    openFacilitiesCount() const;

  // ── Comprobación de factibilidad tras búsqueda local ─────────────────────
  /// Lanza InfeasibleSolutionException si la solución no es factible.
  /// Solo activo en builds de desarrollo (sin NDEBUG).
  /// Se puede eliminar sin consecuencias para el resto del código.
#ifndef NDEBUG
  void checkFeasibilityAfterLS(const Instance& inst,
                               const std::string& context = "") const;
#endif

 private:
  // ── Coste descompuesto (mantenido incrementalmente por los métodos) ───────
  double fixedCost_     = 0.0;
  double transportCost_ = 0.0;
  double totalCost_     = 0.0;
};