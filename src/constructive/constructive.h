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

** Archivo constructive.h: Clase Constructive, base de todos los algoritmos
**/

#ifndef CONSTRUCTIVE_H
#define CONSTRUCTIVE_H

#include "../model/instance.h"
#include "../model/solution.h"

/**
 * @brief Interfaz de estrategia para algoritmos constructivos.
 *
 * Toda clase que construya una solución inicial debe heredar de esta
 * y sobrescribir build().
 */
class Constructive {
 public:
  virtual ~Constructive() = default;
  virtual Solution build(const Instance& inst) = 0;
};

#endif