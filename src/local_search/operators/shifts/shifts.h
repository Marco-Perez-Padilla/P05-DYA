#pragma once
#include "../../local_search.h"

/**
 * @brief Búsqueda local voraz basada en el movimiento Shift(i, j1, j2).
 *
 * Para cada cliente i servido por j1, intenta mover la mayor cantidad
 * posible de su demanda a una instalación abierta j2 más barata,
 * respetando la capacidad residual de j2 y las restricciones de
 * incompatibilidad.
 *
 * Estrategia: mejor mejora (best improvement). En cada iteración se
 * evalúan todos los movimientos posibles y se aplica el de mayor
 * reducción de coste. Se repite hasta alcanzar un óptimo local.
 */
class Shifts : public LocalSearch {
public:
    bool improve(Solution& sol, const Instance& inst) override;
    std::string name() const override { return "ShiftLS"; }
};