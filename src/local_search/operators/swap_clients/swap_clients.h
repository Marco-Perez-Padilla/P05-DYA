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

** Archivo swap_clients.h: Operador de búsqueda local para intercambiar clientes
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      31/03/2026 - Creacion (primera version) del codigo
**/

#ifndef SWAP_CLIENTS_H
#define SWAP_CLIENTS_H

#include "../../local_search.h"

/**
 * @brief Búsqueda local voraz basada en el movimiento Swap-Clientes(client_1, client_2).
 *
 * Para cada par de clientes client_1 e client_2 que estén servidos completamente
 * por una única instalación (|facilities_of[i]| == 1), intenta
 * intercambiar sus instalaciones asignadas.
 */
class SwapClients : public LocalSearch {
 public:
  bool improve(Solution& solution, const Instance& inst) override;
  std::string name() const override { return "SwapClientsLS"; }
};

#endif