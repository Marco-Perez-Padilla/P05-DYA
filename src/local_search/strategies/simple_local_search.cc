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