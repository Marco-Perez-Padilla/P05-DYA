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

** Archivo swap_facilities.h: Operador de búsqueda local para intercambiar instalaciones
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      31/03/2026 - Creacion (primera version) del codigo
**/

#ifndef SWAP_FACILITIES_H
#define SWAP_FACILITIES_H

#include "../../local_search.h"

/**
 * @brief Búsqueda local voraz basada en Swap-Instalaciones(jopen, jclosed).
 *
 * Para cada par (jclose abiertas, jnew cerradas), simula cerrar
 * jclose y abrir jnew, redistribuyendo los clientes de jclose a la
 * instalación abierta (incluida jnew) de menor coste compatible.
 */
class SwapFacilities : public LocalSearch {
 public:
  bool improve(Solution& solution, const Instance& inst) override;
  std::string name() const override { return "SwapFacilitiesLS"; }
};

#endif