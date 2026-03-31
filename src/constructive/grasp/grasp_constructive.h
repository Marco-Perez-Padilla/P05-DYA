#pragma once
#include "../constructive.h"
#include <random>

/**
 * @brief Fase constructiva del GRASP para el MS-CFLP-CI.
 *
 * Introduce aleatoriedad sobre el algoritmo voraz mediante una
 * Lista Restringida de Candidatos (LRC) de tamaño alpha:
 *
 * FASE 1 – Selección de instalaciones:
 *   En lugar de elegir siempre la instalación de menor coste fijo,
 *   se construye una LRC con las alpha mejores candidatas y se
 *   elige una aleatoriamente.
 *
 * FASE 2 – Asignación de clientes:
 *   Los clientes se procesan en orden aleatorio. Para cada cliente,
 *   se construye una LRC con las alpha instalaciones abiertas,
 *   compatibles y con capacidad de menor coste de transporte,
 *   y se elige aleatoriamente.
 */
class GRASPConstructive : public Constructive {
public:
    explicit GRASPConstructive(int alpha = 3, unsigned seed = 42)
        : alpha_(alpha), rng_(seed) {}

    Solution build(const Instance& inst) override;
    void setSeed(unsigned seed) { rng_.seed(seed); }
    int  alpha() const { return alpha_; }

private:
    int         alpha_; ///< Tamaño de la LRC
    std::mt19937 rng_;  ///< Motor de aleatoriedad
    int         k_ = 5; ///< Holgura de instalaciones (igual que el voraz)
};