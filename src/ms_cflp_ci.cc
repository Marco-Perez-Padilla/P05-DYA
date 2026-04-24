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

** Archivo ms_cflp_ci.cc: Punto de entrada del programa
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      31/03/2026 - Creacion (primera version) del codigo
**      24/04/2056 - Version final entregable
**/

#include "help/help_functions.h"
#include "ui/menu_coordinator.h"
#include "exceptions/exceptions.h"
#include <iostream>

int main(int argc, char* argv[]) {
  const int validation = ValidateArguments(argc, argv);
  if (validation == 0) return 0;  
  if (validation != -1) return 1; 

  try {
    ui_launchApplication();
  } catch (const Exceptions& error) {
    std::cerr << "[Error] " << error.what() << "\n";
    return 1;
  }
  return 0;
}