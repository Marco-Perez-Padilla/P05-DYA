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

** Archivo instance.h: Definición de la estructura Instance
**/

#ifndef INSTANCE_H
#define INSTANCE_H

#include <vector>
#include <utility>

/**
 * @brief Datos estáticos de una instancia MS-CFLP-CI.
 */
struct Instance {
  int warehouses = 0;  
  int stores = 0;
  std::vector<double> capacity;  
  std::vector<double> fixed_cost;  
  std::vector<double> demand; 
  std::vector<std::vector<double>> supply_cost;
  int num_incompatibilities = 0;
  std::vector<std::pair<int,int>> incompatible_pairs;
  std::vector<std::vector<int>> incomp_neighbors;
  std::vector<std::vector<bool>> is_incompat;
};

#endif