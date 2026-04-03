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

** Archivo instance_reader.h: Declaración del lector de instancias .dzn
**/

#ifndef INSTANCE_READER_H
#define INSTANCE_READER_H

#include "../model/instance.h"

/**
 * @brief Lee una instancia MS-CFLP-CI desde un fichero .dzn
 */
class InstanceReader {
 public:
  static Instance read(const std::string& filepath);
};

#endif