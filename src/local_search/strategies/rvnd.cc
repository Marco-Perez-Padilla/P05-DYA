/**
** Universidad de La Laguna
** Escuela Superior de Ingenieria y Tecnologia
** Grado en Ingenieria Informatica
** Asignatura: Diseño y Analisis de Algoritmos
** Curso: 3º
** Practica 5: Algoritmos constructivos y búsquedas por entornos
** Autor: Marco Pérez Padilla
** Correo: alu0101469348@ull.edu.es
** Fecha: 11/04/2026

** Archivo rvnd.cc: Implementación de la clase RVND
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      11/04/2026 - Creacion (primera version) del codigo
**/

#include "rvnd.h"
#include <algorithm>

/**
 * @brief Applies a random variable neighborhood descent strategy. It randomly shuffles the order of the provided local search operators and applies them in sequence until no improvement is found, at which point it reshuffles and tries again. The process continues until a full pass through all operators results in no improvement.
 * @param solution The current solution to be improved.
 * @param inst The instance providing the problem data.
 * @return true if at least one improvement was found and applied, false otherwise.
 */
bool RVND::improve(Solution& solution, const Instance& inst) {
  if (neighborhoods_.empty()) return false;

  std::vector<int> indices(neighborhoods_.size());
  for (size_t i = 0; i < indices.size(); ++i) indices[i] = i;

  bool improved = false;
  while (true) {
    std::shuffle(indices.begin(), indices.end(), rng_);
    bool local_improved = false;
    for (int idx : indices) {
      if (neighborhoods_[idx]->improve(solution, inst)) {
        local_improved = true;
        improved = true;
        break;  
      }
    }
    if (!local_improved) break;
  }
  return improved;
}

/**
 * @brief Allows setting a custom order of local search operators to be applied in the improve method. The order is defined by a vector of indices corresponding to the operators in the original list.
 * @param order A vector of integers representing the new order of operators. Each integer should be
 */
void RVND::setOrder(const std::vector<int>& order) {
  if (order.size() != neighborhoods_.size()) return;
  std::vector<std::shared_ptr<LocalSearch>> reordered;
  for (int idx : order) reordered.push_back(neighborhoods_[idx]);
  neighborhoods_ = std::move(reordered);
}