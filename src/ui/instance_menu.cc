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

** Archivo instance_menu.cc: Implementación de las funciones que coordinan la selección de instancias
**                              Resultado de la refactorización para mejorar la organización del código de la interfaz de usuario.
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      24/04/2026 - Creacion (primera version) del codigo
**/

#include "instance_menu.h"
#include "console_utils.h"
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <iostream>

/**
 * @brief Asks the user for the directory where instance files are located, with a default option. This allows the user to specify a custom directory or use the default one where instances are expected to be found.
 * @return The directory path entered by the user or the default if no input is provided.
 */
static std::string ui_askDataDir() {
  return ui_readLineOrDefault("Directorio de instancias [data/input/Public]: ", "data/input/Public");
}

/**
 * @brief Scans a given directory for files with the .dzn extension, which are considered instance files. It collects their paths into a vector and returns it. 
 *        This allows the program to dynamically find all available instances in the specified directory without hardcoding their names.
 */
static std::vector<std::string> ui_findInstances(const std::string& dir) {
  std::vector<std::string> paths;
  if (!std::filesystem::is_directory(dir)) return paths;
  for (const auto& entry : std::filesystem::directory_iterator(dir)) {
    if (entry.path().extension() == ".dzn") {
      paths.push_back(entry.path().string());
    }
  }
  std::sort(paths.begin(), paths.end());
  return paths;
}

/**
 * @brief Parses the user's input for instance selection, which can include individual indices, ranges, or the keyword "all". It validates the input and returns a vector of selected instance indices (0-based).
 * @param input The raw input string from the user specifying which instances to select.
 * @param total The total number of available instances (used for validating indices).
 * @return A vector of selected instance indices (0-based). If the input is invalid, it returns an empty vector.
 */
static std::vector<int> ui_parseSelection(const std::string& input, int total) {
  std::vector<int> selected;
  if (input == "all") {
      selected.resize(total);
      for (int i = 0; i < total; ++i) selected[i] = i;
      return selected;
  }
  std::stringstream ss(input);
  std::string token;
  while (std::getline(ss, token, ',')) {
    token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
    if (token.empty()) continue;
    size_t dash = token.find('-');
    if (dash != std::string::npos) {
      try {
        int a = std::stoi(token.substr(0, dash));
        int b = std::stoi(token.substr(dash + 1));
        for (int i = a; i <= b; ++i)
          if (i >= 1 && i <= total) selected.push_back(i - 1);
      } catch (...) {}
    } else {
      try {
        int v = std::stoi(token);
        if (v >= 1 && v <= total) selected.push_back(v - 1);
      } catch (...) {}
    }
  }
  std::sort(selected.begin(), selected.end());
  selected.erase(std::unique(selected.begin(), selected.end()), selected.end());
  return selected;
}

/**
 * @brief Displays the list of available instances to the user, showing their indices and file names. This helps the user to know which instances they can select for execution.
 * @param paths A vector of file paths for the available instances.
 * @return A formatted list of instances printed to the console, with indices starting from 1 for user-friendly selection.
 */
std::vector<std::string> ui_selectInstances(RunConfig& config) {
  std::string dir = ui_askDataDir();
  auto paths = ui_findInstances(dir);
  if (paths.empty()) {
    throw std::runtime_error("No se encontraron ficheros .dzn en " + dir);
  }
  ui_listInstances(paths);
  std::cout << "\nFormato de selección: all, 1, 1-5, 1,3,7\n";
  std::string sel = ui_readLineOrDefault("Selección [all]: ", "all");
  config.instance_indexes = ui_parseSelection(sel, (int)paths.size());
  if (config.instance_indexes.empty()) {
    config.instance_indexes.resize(paths.size());
    for (int i = 0; i < (int)paths.size(); ++i) config.instance_indexes[i] = i;
  }
  return paths; 
}