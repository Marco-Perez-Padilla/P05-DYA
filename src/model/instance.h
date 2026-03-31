#pragma once
#include <vector>
#include <utility>

/**
 * @brief Datos estáticos de una instancia MS-CFLP-CI.
 *
 * Índices internos en base 0. El lector del fichero .dzn
 * convierte los pares incompatibles de base 1 a base 0.
 */
struct Instance {
    int m = 0;  ///< Número de instalaciones (Warehouses)
    int n = 0;  ///< Número de clientes (Stores)

    std::vector<double> capacity;   ///< s[j]: capacidad de la instalación j
    std::vector<double> fixedCost;  ///< f[j]: coste fijo de apertura de j
    std::vector<double> demand;     ///< d[i]: demanda del cliente i

    /// supplyCost[i][j]: coste unitario de transporte de j a i
    std::vector<std::vector<double>> supplyCost;

    int numIncompatibilities = 0;
    std::vector<std::pair<int,int>> incompatiblePairs; ///< pares incompatibles, base 0

    /// incompNeighbors[i]: lista de clientes incompatibles con i
    std::vector<std::vector<int>> incompNeighbors;

    /// isIncompat[i1][i2]: consulta O(1) de incompatibilidad
    std::vector<std::vector<bool>> isIncompat;
};