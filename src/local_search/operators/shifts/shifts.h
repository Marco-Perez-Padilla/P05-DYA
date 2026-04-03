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

** Archivo shifts.h: Operador de búsqueda local para mover clientes entre instalaciones
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      31/03/2026 - Creacion (primera version) del codigo
**/

#ifndef SHIFTS_H
#define SHIFTS_H

#include "../../local_search.h"

/**
 * @brief Búsqueda local voraz basada en el movimiento Shift(i, warehouse_1, warehouse_2).
 *
 * Para cada cliente i servido por warehouse_1, intenta mover la mayor cantidad
 * posible de su demanda a una instalación abierta warehouse_2 más barata,
 * respetando la capacidad residual de warehouse_2 y las restricciones de
 * incompatibilidad.
 */
class Shifts : public LocalSearch { 
 public:
  bool improve(Solution& solution, const Instance& inst) override;
  std::string name() const override { return "ShiftLS"; }
};

#endif