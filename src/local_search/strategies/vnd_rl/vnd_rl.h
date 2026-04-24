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

** Archivo vnd_rl.h: Declaración de la clase VND_RL, que implementa la búsqueda local VND con aprendizaje por refuerzo
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      24/04/2026 - Creacion (primera version) del codigo
**/

#ifndef VND_RL_H
#define VND_RL_H

#include "../../local_search.h"
#include "reward_strategy.h"
#include <vector>
#include <string>
#include <random>
#include <fstream>
#include <memory>

class VND_RL : public LocalSearch {
 public:
  VND_RL(const std::vector<std::shared_ptr<LocalSearch>>& operators,
         double alpha, double epsilon, const std::string& instanceName,
         std::shared_ptr<RewardStrategy> reward = std::make_shared<BinaryReward>(),
         int maxSinMejora = 5, int maxTotalIter = 10, bool decayEpsilon = false,
         const std::string& qDir = ".");

  bool improve(Solution& solution, const Instance& inst) override;
  std::string name() const override { return "VND_RL"; }

 private:
  int  selectOperator(const std::vector<double>& Q, int iter);
  void logQValues(int iter, const std::vector<double>& Q);

  std::vector<std::shared_ptr<LocalSearch>> operators_;
  double alpha_;
  double epsilon_;
  bool   decayEpsilon_;
  std::shared_ptr<RewardStrategy> reward_;
  std::string instName_;
  std::string qDir_;
  std::ofstream qlog_;
  int maxSinMejora_;
  int maxTotalIter_;
  std::mt19937 rng_;
  std::vector<double> Q_;
};

#endif