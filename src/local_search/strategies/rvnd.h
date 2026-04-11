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

** Archivo rvnd.h: Declaración de la clase RVND (Random Variable Neighborhood Descent)
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      11/04/2026 - Creacion (primera version) del codigo
**/

#ifndef RVND_H
#define RVND_H

#include "../local_search.h"
#include <vector>
#include <memory>
#include <random>
#include <chrono>

/**
 * @brief Random Variable Neighborhood Descent.
 *
 * Aplica una secuencia aleatoria de vecindades, reiniciando tras cada mejora.
 */
class RVND : public LocalSearch {
 private:
  std::vector<std::shared_ptr<LocalSearch>> neighborhoods_;
  std::mt19937 rng_;

  static unsigned timeSeed() {
    return static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
  }

 public:
  explicit RVND(std::vector<std::shared_ptr<LocalSearch>> neighborhoods) : neighborhoods_(std::move(neighborhoods)), rng_(timeSeed()) {}
  bool improve(Solution& solution, const Instance& inst) override;
  std::string name() const override { return "RVND"; }
  void setOrder(const std::vector<int>& order); 
};

#endif