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

** Archivo simple_local_search.h: Busqueda local simple que aplica varias estrategias en secuencia
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      31/03/2026 - Creacion (primera version) del codigo
**      11/04/2026 - Modificación para añadir setOrder
**/

#ifndef SIMPLE_LOCAL_SEARCH_H
#define SIMPLE_LOCAL_SEARCH_H

#include "../../local_search.h"
#include <vector>
#include <memory>

/**
 * @brief Estrategia simple de búsqueda local:
 * aplica varias búsquedas locales en secuencia hasta no mejorar.
 */
class SimpleLocalSearch : public LocalSearch {
 private:
  std::vector<std::shared_ptr<LocalSearch>> operators_;

 public:
  SimpleLocalSearch(std::vector<std::shared_ptr<LocalSearch>> operators) : operators_(std::move(operators)) {}
  bool improve(Solution& solution, const Instance& inst) override;
  std::string name() const override { return "SimpleLocalSearch"; }
  void setOrder(const std::vector<int>& order);
};

#endif