#pragma once
#include "../../local_search.h"

/**
 * @brief Búsqueda local voraz basada en el movimiento Swap-Clientes(i1, i2).
 *
 * Para cada par de clientes i1 e i2 que estén servidos completamente
 * por una única instalación (|facilitiesOf[i]| == 1), intenta
 * intercambiar sus instalaciones asignadas.
 *
 * La restricción de "un único origen" simplifica la comprobación de
 * capacidad e incompatibilidades en el intercambio.
 *
 * Estrategia: mejor mejora (best improvement).
 */
class SwapClients : public LocalSearch {
public:
    bool improve(Solution& sol, const Instance& inst) override;
    std::string name() const override { return "SwapClientsLS"; }
};