#pragma once
#include "../constructive.h"
#include <random>
#include <chrono>

/**
 * @brief Fase constructiva del GRASP para el MS-CFLP-CI.
 *
 * Introduce aleatoriedad sobre el algoritmo voraz mediante una
 * Lista Restringida de Candidatos (LRC) de tamaño alpha.
 *
 * La semilla del generador se basa en el reloj de alta resolución,
 * por lo que cada llamada a build() produce una solución distinta
 * sin necesidad de gestionar semillas externamente.
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
  explicit GRASPConstructive(int alpha = 3)
      : alpha_(alpha), rng_(timeSeed()) {}

  Solution build(const Instance& inst) override;

  int alpha() const { return alpha_; }

  /// Permite fijar una semilla concreta (útil para tests reproducibles).
  void setSeed(unsigned seed) { rng_.seed(seed); }

  /// Resiembra con el tiempo actual (comportamiento por defecto entre iteraciones).
  void reseed() { rng_.seed(timeSeed()); }

private:
  int          alpha_;
  std::mt19937 rng_;
  int          k_ = 5; ///< Holgura de instalaciones extra

  /// Genera una semilla a partir del reloj de alta resolución.
  static unsigned timeSeed() {
    return static_cast<unsigned>(
        std::chrono::high_resolution_clock::now()
            .time_since_epoch().count());
  }
};