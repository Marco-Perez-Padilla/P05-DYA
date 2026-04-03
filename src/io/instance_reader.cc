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

** Archivo instance_reader.cc: Implementación del lector de instancias .dzn
**/

#include "instance_reader.h"
#include "../exceptions/exceptions.h"
#include <fstream>
#include <sstream>
#include <algorithm>

/**
 * @brief Reads the entire content of a file into a string. Throws exceptions if the file cannot be opened or is empty.
 * @param path The file path to read from.
 * @return The content of the file as a string.
 * @throws FileNotFoundException if the file cannot be opened.
 * @throws EmptyFileException if the file is empty or has no valid content.
 */
static std::string readFile(const std::string& path) {
  std::ifstream file(path);
  if (!file) throw FileNotFoundException(path);

  std::string content{std::istreambuf_iterator<char>(file), {}};
  if (content.empty()) throw EmptyFileException(path);
  return content;
}

/**
 * @brief Extracts the value associated with a given key from the content string. 
 * @param content The string content to search within.
 * @param key The key whose value is to be extracted.
 * @return The value associated with the key as a string.
 * @throws InvalidDataException if the key is not found or if the expected format is not met
 */
static std::string extractValue(const std::string& content, const std::string& key) {
  for (const std::string& search : {key + " =", key + "="}) {
    size_t pos = content.find(search);
    if (pos == std::string::npos) continue;
    size_t eqPos   = content.find('=', pos) + 1;
    size_t semiPos = content.find(';', eqPos);
    if (semiPos == std::string::npos)
      throw InvalidDataException("Falta ';' para la clave: " + key);
    return content.substr(eqPos, semiPos - eqPos);
  }
  throw InvalidDataException("Clave no encontrada en la instancia: " + key);
}

/**
 * @brief Trims leading and trailing whitespace characters from a string.
 * @param string The string to be trimmed.
 * @return A new string with leading and trailing whitespace removed.
 */
static std::string trim(const std::string& string) {
  const std::string whitespace = " \trimmed\n\r";
  size_t first = string.find_first_not_of(whitespace);
  if (first == std::string::npos) return "";
  size_t last = string.find_last_not_of(whitespace);
  return string.substr(first, last - first + 1);
}

/**
 * @brief Parses a scalar integer value from a string, trimming whitespace. 
 * @param val The string containing the scalar value to parse.
 * @return The parsed integer value.
 */
static int parseScalar(const std::string& val) {
  return std::stoi(trim(val));
}


/**
 * @brief Parses a 1D array from a string in the format "[v0, v1, v2,...]". Trims whitespace and converts values to doubles.
 * @param val The string containing the 1D array to parse.
 * @return A vector of doubles representing the parsed 1D array.
 */
static std::vector<double> parse1DArray(const std::string& val) {
  size_t start = val.find('[') + 1;
  size_t end   = val.rfind(']');
  const std::string inner = val.substr(start, end - start);

  std::vector<double> result;
  std::stringstream ss(inner);
  std::string token;
  while (std::getline(ss, token, ',')) {
    const std::string trimmed = trim(token);
    if (!trimmed.empty()) result.push_back(std::stod(trimmed));
  }
  return result;
}

/**
 * @brief Parses a 2D array from a string in the format "[| r1c1, r1c2 | r2c1, r2c2 | ...]". Trims whitespace and converts values to doubles. Rows are separated by '|'.
 * @param val The string containing the 2D array to parse.
 * @return A vector of vectors of doubles representing the parsed 2D array.
 */
static std::vector<std::vector<double>> parse2DArray(const std::string& val) {
  size_t start = val.find('[') + 1;
  size_t end   = val.rfind(']');
  const std::string inner = val.substr(start, end - start);

  std::vector<std::vector<double>> result;
  std::stringstream ss(inner);
  std::string row_string;
  while (std::getline(ss, row_string, '|')) {
    const std::string trimmed = trim(row_string);
    if (trimmed.empty()) continue;

    std::vector<double> row;
    std::stringstream rs(trimmed);
    std::string token;
    while (std::getline(rs, token, ',')) {
      const std::string tk = trim(token);
      if (!tk.empty()) row.push_back(std::stod(tk));
    }
    if (!row.empty()) result.push_back(row);
  }
  return result;
}

/**
 * @brief Parses a list of incompatible client pairs from a string in the format "[| client_1, warehouse_1 | client_2, warehouse_2 | ...]". Trims whitespace and converts values to integers. Each pair is expected to have exactly two integers.
 * @param val The string containing the incompatible pairs to parse.
 * @return A vector of pairs of integers representing the incompatible client pairs, with client indexes converted
 */
static std::vector<std::pair<int,int>> parsePairs(const std::string& val) {
  const auto rows = parse2DArray(val);
  std::vector<std::pair<int,int>> result;
  result.reserve(rows.size());
  for (auto& row : rows) {
    if (row.size() == 2) {
      result.push_back({(int)row[0] - 1, (int)row[1] - 1}); 
    }
  }
  return result;
}

/**
 * @brief Reads an instance from a .dzn file specified by the filepath. It reads the file content, extracts and parses all required fields to construct an Instance object, and returns it. The method handles parsing of scalars, 1D arrays, 2D arrays, and incompatible pairs, as well as constructing auxiliary data structures for quick compatibility checks.
 * @param filepath The path to the .dzn file containing the instance data.
 * @return An Instance object populated with the data from the file.
 */
Instance InstanceReader::read(const std::string& filepath) {
  const std::string content = readFile(filepath);

  Instance inst;
  inst.warehouses = parseScalar(extractValue(content, "Warehouses"));
  inst.stores = parseScalar(extractValue(content, "Stores"));

  inst.capacity   = parse1DArray(extractValue(content, "Capacity"));
  inst.fixed_cost  = parse1DArray(extractValue(content, "FixedCost"));
  inst.demand     = parse1DArray(extractValue(content, "Goods"));
  inst.supply_cost = parse2DArray(extractValue(content, "SupplyCost"));

  inst.num_incompatibilities = parseScalar(extractValue(content, "Incompatibilities"));
  inst.incompatible_pairs    = parsePairs(extractValue(content, "IncompatiblePairs"));

  inst.incomp_neighbors.assign(inst.stores, {});
  inst.is_incompat.assign(inst.stores, std::vector<bool>(inst.stores, false));

  for (auto& [client_1, client_2] : inst.incompatible_pairs) {
    inst.incomp_neighbors[client_1].push_back(client_2);
    inst.incomp_neighbors[client_2].push_back(client_1);
    inst.is_incompat[client_1][client_2] = true;
    inst.is_incompat[client_2][client_1] = true;
  }

  return inst;
}