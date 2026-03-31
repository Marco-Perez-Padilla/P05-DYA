#pragma once
#include <string>
#include "../model/instance.h"
#include "../model/solution.h"

/**
 * @brief Interfaz de estrategia para búsquedas locales.
 *
 * improve() aplica repetidamente el mejor movimiento de mejora del
 * entorno correspondiente hasta alcanzar un óptimo local.
 * Devuelve true si encontró al menos una mejora.
 */
class LocalSearch {
public:
    virtual ~LocalSearch() = default;
    virtual bool improve(Solution& sol, const Instance& inst) = 0;
    virtual std::string name() const = 0;
};