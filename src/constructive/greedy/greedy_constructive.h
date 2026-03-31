#pragma once
#include "../constructive.h"

/**
 * @brief Algoritmo voraz para el MS-CFLP-CI (Algoritmo 1 de la práctica).
 *
 * FASE 1 – Selección de instalaciones:
 *   Ordena las instalaciones por coste fijo ascendente y las abre
 *   hasta cubrir la demanda total. Añade k instalaciones extra como
 *   holgura para las restricciones de incompatibilidad.
 *
 * FASE 2 – Asignación de clientes:
 *   Para cada cliente (en orden de índice), ordena las instalaciones
 *   abiertas por coste de transporte ascendente y asigna de forma
 *   voraz, verificando capacidad residual e incompatibilidades.
 */
class GreedyConstructive : public Constructive {
public:
    explicit GreedyConstructive(int slackK = 5) : k_(slackK) {}
    Solution build(const Instance& inst) override;

private:
    int k_; ///< Número de instalaciones extra de holgura
};