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

** Archivo console_utils.cc: Implementación de las funciones de utilidad para la interfaz de consola
**                              Resultado de la refactorización para mejorar la organización del código de la interfaz de usuario.
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      24/04/2026 - Creacion (primera version) del codigo
**/

#include "console_utils.h"
#include <iostream>
#include <limits>
#include <sstream>
#include <algorithm>
#include <filesystem>

/**
 * @brief Reads a line of input from the user, with a prompt and a default value. If the user enters an empty line, the default value is returned. The function also trims leading and trailing whitespace from the input.
 * @param prompt The message to display to the user when asking for input.
 * @param defaultVal The default value to return if the user enters an empty line.
 * @return The user's input with whitespace trimmed, or the default value if the input is empty.
 */
std::string ui_readLineOrDefault(const std::string& prompt, const std::string& defaultVal) {
  std::cout << prompt;
  std::string line;
  std::getline(std::cin, line);
  line.erase(0, line.find_first_not_of(" \t\n\r"));
  line.erase(line.find_last_not_of(" \t\n\r") + 1);
  if (line.empty()) return defaultVal;
  return line;
}

/**
 * @brief Asks the user to select an option from a range of integers. The user is prompted with a message and must enter a number between min and max (inclusive). 
 *        If the user enters an invalid input, they are prompted again until a valid option is selected.
 * @param prompt The message to display to the user when asking for input.
 * @param min The minimum valid integer option.
 * @param max The maximum valid integer option.
 * @param defaultVal The default value to return if the user enters an empty line.
 * @return The integer option selected by the user, or the default value if the input is empty.
 */
int ui_askOption(const std::string& prompt, int min, int max, int defaultVal) {
  while (true) {
    std::string line = ui_readLineOrDefault(prompt, std::to_string(defaultVal));
    try {
      int val = std::stoi(line);
      if (val >= min && val <= max) return val;
    } catch (...) {}
    std::cout << "  Entrada no válida. Debe ser un número entre " << min << " y " << max << ".\n";
  }
}

/**
 * @brief Asks the user a yes/no question and returns a boolean value. The user is prompted with a message and must enter 'S' for yes or 'N' for no.
 *       If the user enters an invalid input, they are prompted again until a valid response is given.
 * @param prompt The message to display to the user when asking for input.
 * @param defaultYes The default value to return if the user enters an empty line (true for yes, false for no).
 * @return true if the user answers yes ('S'), false if the user answers no ('N'), or the default value if the input is empty.
 */
bool ui_askYesNo(const std::string& prompt, bool defaultYes) {
  std::string defStr = defaultYes ? "S" : "N";
  while (true) {
    std::string line = ui_readLineOrDefault(prompt + " [S/N] (por defecto " + defStr + "): ", defStr);
    if (line == "S" || line == "s") return true;
    if (line == "N" || line == "n") return false;
    std::cout << "  Responda S o N.\n";
  }
}

/**
 * @brief Asks the user to enter an integer value with a prompt and a default value. The function validates that the input is a valid integer and prompts the user again if the input is invalid.
 * @param prompt The message to display to the user when asking for input.
 * @param defaultVal The default integer value to return if the user enters an empty line.
 * @return The integer value entered by the user, or the default value if the input is empty.
 */
int ui_askInt(const std::string& prompt, int defaultVal) {
  while (true) {
    std::string line = ui_readLineOrDefault(prompt, std::to_string(defaultVal));
    try {
      return std::stoi(line);
    } catch (...) {
      std::cout << "  Introduzca un número entero.\n";
    }
  }
}

/**
 * @brief Asks the user to enter a double value with a prompt and a default value. The function validates that the input is a valid double and prompts the user again if the input is invalid.
 * @param prompt The message to display to the user when asking for input.
 * @param defaultVal The default double value to return if the user enters an empty line.
 * @return The double value entered by the user, or the default value if the input is
 */
double ui_askDouble(const std::string& prompt, double defaultVal) {
  while (true) {
    std::string line = ui_readLineOrDefault(prompt, std::to_string(defaultVal));
    try {
      return std::stod(line);
    } catch (...) {
      std::cout << "  Introduzca un número decimal.\n";
    }
  }
}

/**
 * @brief Asks the user to enter a file path with a prompt and a default value. The function does not validate the existence of the file or directory, but simply returns the user's input or the default value if the input is empty.
 * @param prompt The message to display to the user when asking for input.
 * @param defaultVal The default file path to return if the user enters an empty line.
 * @return The file path entered by the user, or the default value if the input is
 */
void ui_listInstances(const std::vector<std::string>& paths) {
  std::cout << "\nInstancias disponibles:\n";
  for (size_t i = 0; i < paths.size(); ++i) {
    std::cout << "  [" << (i + 1) << "] "
              << std::filesystem::path(paths[i]).filename().string() << "\n";
  }
}

/**
 * @brief Asks the user to enter a file path with a prompt and a default value. The function does not validate the existence of the file or directory, but simply returns the user's input or the default value if the input is empty.
 * @param prompt The message to display to the user when asking for input.
 * @param defaultVal The default file path to return if the user enters an empty line.
 * @return The file path entered by the user, or the default value if the input is
 */
std::string ui_askFilePath(const std::string& prompt, const std::string& defaultVal) {
  return ui_readLineOrDefault(prompt, defaultVal);
}