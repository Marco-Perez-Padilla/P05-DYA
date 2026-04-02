/** Universidad de La Laguna
** Escuela Superior de Ingenieria y Tecnologia
** Grado en Ingenieria Informatica
** Asignatura: Diseño y Analisis de Algoritmos
** Curso: 3º
** Practica 5: Algoritmos constructivos y búsquedas por entornos
** Autor: Marco Pérez Padilla
** Correo: alu0101469348@ull.edu.es
** Fecha: 31/03/2026

** Archivo grasp_constructive.h: Clase GRASPConstructive para el MS-CFLP-CI
**/

#ifndef GRASP_CONSTRUCTIVE_H
#define GRASP_CONSTRUCTIVE_H

#include "../constructive.h"
#include <random>
#include <chrono>

/**
 * @brief Fase constructiva del GRASP para el MS-CFLP-CI.
 *
 * Introduce aleatoriedad sobre el algoritmo voraz mediante una
 * Lista Restringida de Candidatos (LRC) de tamaño alpha.
 *
 * La semilla del generador se basa en el reloj de alta resolución,
 * por lo que cada llamada a build() produce una solución distinta
 * sin necesidad de gestionar semillas externamente.
 *
 * FASE 1 – Selección de instalaciones:
 *   En lugar de elegir siempre la instalación de menor coste fijo,
 *   se construye una LRC con las alpha mejores candidatas y se
 *   elige una aleatoriamente.
 *
 * FASE 2 – Asignación de clientes:
 *   Los clientes se procesan en orden aleatorio. Para cada cliente,
 *   se construye una LRC con las alpha instalaciones abiertas,
 *   compatibles y con capacidad de menor coste de transporte,
 *   y se elige aleatoriamente.
 */
class GRASPConstructive : public Constructive {
 private:
  int alpha_;
  std::mt19937 rng_;
  int holgura_ = 5;
  static unsigned timeSeed() {return static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count());}

 public:
  explicit GRASPConstructive(int alpha = 3) : alpha_(alpha), rng_(timeSeed()) {}
  Solution build(const Instance& inst) override;
  int alpha() const { return alpha_; }
  void setSeed(unsigned seed) { rng_.seed(seed); }
  void reseed() { rng_.seed(timeSeed()); }
};

#endif