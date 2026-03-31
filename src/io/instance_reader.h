#pragma once
#include <string>
#include "../model/instance.h"

/**
 * @brief Lee una instancia MS-CFLP-CI desde un fichero .dzn (formato MiniZinc).
 *
 * El formato esperado es:
 *   Warehouses = <int>;
 *   Stores     = <int>;
 *   Capacity   = [<int>, ...];
 *   FixedCost  = [<int>, ...];
 *   Goods      = [<int>, ...];
 *   SupplyCost = [|row0|row1|...|];   (Stores filas × Warehouses columnas)
 *   Incompatibilities  = <int>;
 *   IncompatiblePairs  = [|i1, i2|...|];  (base 1, se convierte a base 0)
 */
class InstanceReader {
public:
    static Instance read(const std::string& filepath);
};