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

** Archivo local_search.h: Interfaz para búsquedas locales
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      31/03/2026 - Creacion (primera version) del codigo
**/

#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include <string>
#include "../model/instance.h"
#include "../model/solution.h"

/**
 * @brief Interfaz de estrategia para búsquedas locales.
 */
class LocalSearch {
 public:
  virtual ~LocalSearch() = default;
  virtual bool improve(Solution& solution, const Instance& inst) = 0;
  virtual std::string name() const = 0;
};

#endif