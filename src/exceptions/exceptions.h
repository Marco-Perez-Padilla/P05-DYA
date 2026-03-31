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

** Archivo exceptions.h: Archivo de excepciones
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      31/03/2026 - Creacion (primera version) del codigo
**/

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

/**
 * @brief Base exception class for this practice
 */
class Exceptions : public std::exception {
 protected:
  std::string error_message_;
 public:
  explicit Exceptions(const std::string& msg) : error_message_(msg) {}
  const char* what() const noexcept override { return error_message_.c_str(); }
};

/**
 * @brief Thrown when a file cannot be opened
 */
class FileNotFoundException : public Exceptions {
 public:
  explicit FileNotFoundException(const std::string& filename)
    : Exceptions("Cannot open file: '" + filename + "'") {}
};

/**
 * @brief Thrown when a file has invalid or empty content
 */
class EmptyFileException : public Exceptions {
 public:
  explicit EmptyFileException(const std::string& filename)
    : Exceptions("File is empty or has no valid content: '" + filename + "'") {}
};

/**
 * @brief Thrown when a solution is not feasible
 */
class InfeasibleSolutionException : public Exceptions {
 public:
  explicit InfeasibleSolutionException(const std::string& reason)
    : Exceptions("Solution is infeasible: " + reason) {}
};

/**
 * @brief Thrown when data is inconsistent
 */
class InvalidDataException : public Exceptions {
 public:
  explicit InvalidDataException(const std::string& reason)
    : Exceptions("Invalid data: " + reason) {}
};

#endif