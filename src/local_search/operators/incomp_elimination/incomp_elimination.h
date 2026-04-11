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

** Archivo incomp_elimination.h: Operador de búsqueda local para eliminación de incompatibilidades
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      11/04/2026 - Creacion (primera version) del codigo
**/

#ifndef INCOMP_ELIMINATION_H
#define INCOMP_ELIMINATION_H

#include "../../local_search.h"

/**
 * @brief Búsqueda local basada en "Eliminación de Incompatibilidad".
 *
 * Identifica una instalación abierta con alto coste de transporte por unidad
 * de capacidad y mueve al cliente que más bloqueos por incompatibilidad
 * causa en dicha instalación hacia otra instalación compatible.
 */
class IncompElimination : public LocalSearch {
 public:
  bool improve(Solution& solution, const Instance& inst) override;
  std::string name() const override { return "IncompEliminationLS"; }
};

#endif