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

** Archivo console_utils.h: Definición de las funciones de utilidad para la interfaz de consola
**                              Resultado de la refactorización para mejorar la organización del código de la interfaz de usuario.
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      24/04/2026 - Creacion (primera version) del codigo
**/

#ifndef UI_CONSOLE_UTILS_H
#define UI_CONSOLE_UTILS_H

#include <string>
#include <vector>

std::string ui_readLineOrDefault(const std::string& prompt, const std::string& defaultVal);
int ui_askOption(const std::string& prompt, int min, int max, int defaultVal);
bool ui_askYesNo(const std::string& prompt, bool defaultYes);
int ui_askInt(const std::string& prompt, int defaultVal);
double ui_askDouble(const std::string& prompt, double defaultVal);
std::string ui_askFilePath(const std::string& prompt, const std::string& defaultVal);
void ui_listInstances(const std::vector<std::string>& paths);

#endif