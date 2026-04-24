/**
** Universidad de La Laguna
** Escuela Superior de Ingenieria y Tecnologia
** Grado en Ingenieria Informatica
** Asignatura: Diseño y Analisis de Algoritmos
** Curso: 3º
** Practica 5: Algoritmos constructivos y búsquedas por entornos
** Autor: Marco Pérez Padilla
** Correo: alu0101469348@ull.edu.es
** Fecha: 24/04/2026

** Archivo instance_menu.h: Definición de las funciones que coordinan la selección de instancias
**                              Resultado de la refactorización para mejorar la organización del código de la interfaz de usuario.
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      24/04/2026 - Creacion (primera version) del codigo
**/

#ifndef UI_INSTANCE_MENU_H
#define UI_INSTANCE_MENU_H

#include <string>
#include <vector>
#include "../model/run_config.h"

std::vector<std::string> ui_selectInstances(RunConfig& config);

#endif