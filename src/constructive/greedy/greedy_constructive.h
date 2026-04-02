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

** Archivo greedy_constructive.h: Clase GreedyConstructive para el MS-CFLP-CI
**/

#ifndef GREEDY_CONSTRUCTIVE_H
#define GREEDY_CONSTRUCTIVE_H

#include "../constructive.h"

/**
 * @brief Algoritmo voraz para el MS-CFLP-CI (Algoritmo 1 de la práctica).
 *
 * FASE 1 – Selección de instalaciones:
 *   Ordena las instalaciones por coste fijo ascendente y las abre
 *   hasta cubrir la demanda total. Añade holgura instalaciones extra como
 *   holgura para las restricciones de incompatibilidad.
 *
 * FASE 2 – Asignación de clientes:
 *   Para cada cliente (en orden de índice), ordena las instalaciones
 *   abiertas por coste de transporte ascendente y asigna de forma
 *   voraz, verificando capacidad residual e incompatibilidades.
 */
class GreedyConstructive : public Constructive {
 private:
  int holgura_;

 public:
  explicit GreedyConstructive(int slackK = 5) : holgura_(slackK) {}
  Solution build(const Instance& inst) override;
};

#endif