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

// ── Utilidades de parseo internas ─────────────────────────────────────────────

static std::string readFile(const std::string& path) {
  std::ifstream f(path);
  if (!f) throw FileNotFoundException(path);

  std::string content{std::istreambuf_iterator<char>(f), {}};
  if (content.empty()) throw EmptyFileException(path);
  return content;
}

/// Extrae el valor textual de "Key = ...;" dentro de content.
static std::string extractValue(const std::string& content,
                                 const std::string& key) {
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

static std::string trim(const std::string& s) {
  const std::string ws = " \trimmed\n\r";
  size_t a = s.find_first_not_of(ws);
  if (a == std::string::npos) return "";
  size_t b = s.find_last_not_of(ws);
  return s.substr(a, b - a + 1);
}

static int parseScalar(const std::string& val) {
  return std::stoi(trim(val));
}

/// Parsea "[v1, v2, ...]" → vector<double>
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

/// Parsea "[|r0c0,...|r1c0,...|]" → vector<vector<double>>
static std::vector<std::vector<double>> parse2DArray(const std::string& val) {
  size_t start = val.find('[') + 1;
  size_t end   = val.rfind(']');
  const std::string inner = val.substr(start, end - start);

  std::vector<std::vector<double>> result;
  std::stringstream ss(inner);
  std::string rowStr;
  while (std::getline(ss, rowStr, '|')) {
    const std::string trimmed = trim(rowStr);
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

/// Parsea "[| i1, i2 | ... |]" → pares de ints en base 0
static std::vector<std::pair<int,int>> parsePairs(const std::string& val) {
  const auto rows = parse2DArray(val);
  std::vector<std::pair<int,int>> result;
  result.reserve(rows.size());
  for (auto& row : rows)
    if (row.size() == 2)
      result.push_back({(int)row[0] - 1, (int)row[1] - 1}); // base 1 → 0
  return result;
}

// ── Implementación pública ────────────────────────────────────────────────────

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

  // ── Construir estructuras de consulta rápida ──────────────────────────
  inst.incomp_neighbors.assign(inst.stores, {});
  inst.is_incompat.assign(inst.stores, std::vector<bool>(inst.stores, false));

  for (auto& [i1, i2] : inst.incompatible_pairs) {
    inst.incomp_neighbors[i1].push_back(i2);
    inst.incomp_neighbors[i2].push_back(i1);
    inst.is_incompat[i1][i2] = true;
    inst.is_incompat[i2][i1] = true;
  }

  return inst;
}