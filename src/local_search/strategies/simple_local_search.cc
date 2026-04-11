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

** Archivo simple_local_search.cc: Implementación de la estrategia de búsqueda local simple
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      31/03/2026 - Creacion (primera version) del codigo
**      11/04/2026 - Modificación para añadir setOrder
**/

#include "simple_local_search.h"

/**
 * @brief Applies multiple local search operators in sequence until no improvement is found, effectively performing a basic VND (without dynamic order change).
 * @param solution The current solution to be improved.
 * @param inst The instance providing the problem data.
 * @return true if at least one improvement was found, false otherwise.
 */
bool SimpleLocalSearch::improve(Solution& solution, const Instance& inst) {
  bool any_improved = false;
  bool improved = true;

  while (improved) {
    improved = false;
    for (auto& search_operator : operators_) {
      if (search_operator->improve(solution, inst)) {
        improved = true;
        any_improved = true;
      }
    }
  }

  return any_improved;
}

/**
 * @brief Allows setting a custom order of local search operators to be applied in the improve method. The order is defined by a vector of indices corresponding to the operators in the original list.
 * @param order A vector of integers representing the new order of operators. Each integer should be
 */
void SimpleLocalSearch::setOrder(const std::vector<int>& order) {
  if (order.size() != operators_.size()) return;
  std::vector<std::shared_ptr<LocalSearch>> reordered;
  for (int idx : order) {
    if (idx >= 0 && idx < static_cast<int>(operators_.size())) {
      reordered.push_back(operators_[idx]);
    }
  }
  if (reordered.size() == operators_.size()) {
    operators_ = std::move(reordered);
  }
}