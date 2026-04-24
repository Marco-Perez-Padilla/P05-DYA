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

** Archivo gvns.h: Declaración de la clase GVNS, que implementa el algoritmo General Variable Neighborhood Search para MS-CFLP-CI
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      24/04/2026 - Creacion (primera version) del codigo
**/

#ifndef GVNS_H
#define GVNS_H

#include "../../model/instance.h"
#include "../../model/solution.h"
#include "../../constructive/constructive.h"
#include "../../local_search/local_search.h"
#include <memory>
#include <random>

class GVNS {
 public:
  GVNS(std::shared_ptr<Constructive> constructive, std::shared_ptr<LocalSearch> improvement, int kmax, int maxIter);
  Solution run(const Instance& inst);
  void setSeed(unsigned seed) { rng_.seed(seed); }

 private:
  void shake(Solution& solution, int k, const Instance& inst);
  std::shared_ptr<Constructive> constructive_;
  std::shared_ptr<LocalSearch> improvement_;
  int kmax_;
  int maxIter_;
  std::mt19937 rng_;
};

#endif