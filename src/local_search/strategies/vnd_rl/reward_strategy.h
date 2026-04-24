/**
** Universidad de La Laguna
** Escuela Superior de Ingenieria y Tecnologia
** Grado en Ingenieria Informatica
** Asignatura: Diseño y Analisis de Algoritmos
** Curso: 3º
** Practica 5: Algoritmos constructivos y búsquedas por entornos
** Autor: Marco Pérez Padilla
** Correo: alu0101469348@ull.edu.es
** Fecha: 24/04/2026

** Archivo reward_strategy.h: Declaración de la clase RewardStrategy y sus derivadas para el VND‑RL
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      24/04/2026 - Creacion (primera version) del codigo
**/

#ifndef REWARD_STRATEGY_H
#define REWARD_STRATEGY_H

#include <string>

class RewardStrategy {
 public:
  virtual ~RewardStrategy() = default;
  virtual double compute(double oldCost, double newCost) const = 0;
  virtual std::string name() const = 0;
};

/**
 * @brief Binary reward: returns 1.0 if newCost < oldCost (improvement), otherwise 0.0
 */
class BinaryReward : public RewardStrategy {
 public:
  double compute(double oldCost, double newCost) const override {
    return (newCost < oldCost - 1e-6) ? 1.0 : 0.0;
  }
  std::string name() const override { return "Binary"; }
};

/**
 * @brief Proportional reward: returns a value between 0 and 1 proportional to the relative improvement (oldCost - newCost) / oldCost, or 0 if there is no improvement.
 */
class ProportionalReward : public RewardStrategy {
 public:
  double compute(double oldCost, double newCost) const override {
    if (newCost < oldCost - 1e-6) {
      return (oldCost - newCost) / oldCost;
    }
    return 0.0;
  }
  std::string name() const override { return "Proportional"; }
};

/**
 * @brief Normalized reward: returns a value between 0 and 1 proportional to the improvement (oldCost - newCost) normalized by a maximum expected improvement (maxDelta). 
 *        If maxDelta is 0, it behaves like a binary reward.
 */
class NormalizedReward : public RewardStrategy {
 public:
  explicit NormalizedReward(double maxDelta = 1.0) : maxDelta_(maxDelta) {}

  double compute(double oldCost, double newCost) const override {
    if (newCost < oldCost - 1e-6) {
      double delta = oldCost - newCost;
      return (maxDelta_ > 0.0) ? (delta / maxDelta_) : 1.0;
    }
    return 0.0;
  }
  std::string name() const override { return "Normalized"; }

 private:
  double maxDelta_;
};


#endif