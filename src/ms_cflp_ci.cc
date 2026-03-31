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
**/

#include <iostream>
#include "help/help_functions.h"
#include "menus/menu.h"
#include "exceptions/exceptions.h"

int main(int argc, char* argv[]) {
  const int validation = ValidateArguments(argc, argv);
  if (validation == 0) return 0;   // --help: salir limpiamente
  if (validation != -1) return 1;  // argumentos incorrectos

  try {
    Menu menu(argv[1]);
    menu.run();
  } catch (const Exceptions& e) {
    std::cerr << "[Error] " << e.what() << "\n";
    return 1;
  }

  return 0;
}