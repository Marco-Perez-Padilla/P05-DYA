#pragma once
#include "../../local_search.h"

/**
 * @brief Búsqueda local voraz basada en Swap-Instalaciones(jopen, jclosed).
 *
 * Para cada par (jclose ∈ abiertas, jnew ∈ cerradas), simula cerrar
 * jclose y abrir jnew, redistribuyendo los clientes de jclose a la
 * instalación abierta (incluida jnew) de menor coste compatible.
 *
 * También evalúa el cierre puro de una instalación (sin abrir otra)
 * cuando sus clientes pueden redistribuirse hacia otras instalaciones
 * ya abiertas.
 *
 * Estrategia: primera mejora (first improvement) para reducir la
 * complejidad de almacenar el plan de redistribución.
 *
 * PROPIEDAD CLAVE: dado que la solución es factible, todos los clientes
 * de jclose son mutuamente compatibles. Por tanto, al moverlos a una
 * instalación destino solo hace falta verificar la compatibilidad con
 * los clientes ya existentes en dicho destino (incompCount[i][j] == 0).
 */
class SwapFacilities : public LocalSearch {
public:
    bool improve(Solution& sol, const Instance& inst) override;
    std::string name() const override { return "SwapFacilitiesLS"; }
};