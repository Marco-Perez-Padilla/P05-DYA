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

** Archivo vnd_rl.cc: Implementación de la clase VND_RL, que implementa la búsqueda local VND con aprendizaje por refuerzo
**
** Referencias:
**      Enlaces de interes

** Historial de revisiones:
**      24/04/2026 - Creacion (primera version) del codigo
**/

#include "vnd_rl.h"
#include <algorithm>
#include <iomanip>
#include <cmath>

#include "vnd_rl.h"
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <filesystem>

/**
 * @brief Constructor of the VND_RL class, which initializes the local search operators, learning parameters (alpha, epsilon), instance name for logging, reward strategy, and other settings. 
 *        It also prepares a CSV file for logging the evolution of Q-values over iterations.
 * @param operators A vector of shared pointers to LocalSearch objects that represent the different local search operators to be used.
 * @param alpha The learning rate for updating Q-values based on observed rewards.
 * @param epsilon The exploration rate for the epsilon-greedy strategy in operator selection.
 * @param instanceName The name of the instance being solved, used for logging purposes.
 * @param reward A shared pointer to a RewardStrategy object that defines how rewards are computed based on cost improvements.
 * @param maxSinMejora The maximum number of iterations without improvement before stopping the local search.
 * @param maxTotalIter The maximum total number of iterations for the local search.
 * @param decayEpsilon A boolean indicating whether epsilon should decay over iterations to reduce exploration as the search progresses.
 * @param qDir The directory where the Q-values evolution log will be stored.
 */
VND_RL::VND_RL(const std::vector<std::shared_ptr<LocalSearch>>& operators,
               double alpha, double epsilon, const std::string& instanceName,
               std::shared_ptr<RewardStrategy> reward, int maxSinMejora,
               int maxTotalIter, bool decayEpsilon, const std::string& qDir)
    : operators_(operators), alpha_(alpha), epsilon_(epsilon), decayEpsilon_(decayEpsilon),
      reward_(reward), instName_(instanceName), qDir_(qDir), maxSinMejora_(maxSinMejora),
      maxTotalIter_(maxTotalIter), rng_(std::random_device{}()), Q_(operators_.size(), 0.5) {

  std::filesystem::create_directories(qDir_);
  std::string filename = qDir_ + "/q_evolution_" + instName_ + ".csv";
  qlog_.open(filename);
  qlog_ << "iter";
  for (size_t i = 0; i < operators_.size(); ++i)
    qlog_ << ",LS" << (i + 1);
  qlog_ << "\n";
}

/**
 * @brief Implements the improve method of the LocalSearch interface. This method performs the VND with Reinforcement Learning to try to improve the given solution. 
 *        It iteratively selects operators based on Q-values, applies them, computes rewards, and updates Q-values until a stopping criterion is met (no improvement for a certain number of iterations or reaching a maximum total number of iterations).
 * @param solution The solution to be improved.
 * @param inst The instance of the problem being solved, which may be needed for the local search operators to evaluate moves and for feasibility checks.
 * @return A boolean indicating whether the solution was improved during the process.
 */
bool VND_RL::improve(Solution& solution, const Instance& inst) {
  if (operators_.empty()) return false;

  bool globalImproved = false;
  int sinMejora = 0, totalIter = 0;

  logQValues(totalIter, Q_);

  while (sinMejora < maxSinMejora_ && totalIter < maxTotalIter_) {
    int chosen = selectOperator(Q_, totalIter);
    double oldCost = solution.getTotalCost();
    bool improved = operators_[chosen]->improve(solution, inst);
    double newCost = solution.getTotalCost();
    double r = reward_->compute(oldCost, newCost);

    Q_[chosen] = Q_[chosen] + alpha_ * (r - Q_[chosen]);

    ++totalIter;
    logQValues(totalIter, Q_);

    if (improved) {
      globalImproved = true;
      sinMejora = 0;
    } else {
      ++sinMejora;
    }
  }
  return globalImproved;
}

/**
 * @brief Selects a local search operator based on the current Q-values and the epsilon-greedy strategy. With probability epsilon, it selects a random operator (exploration), and with probability 1-epsilon, 
 *        it selects the operator with the highest Q-value (exploitation). If decayEpsilon_ is true, epsilon decreases over iterations to reduce exploration as the search progresses.
 * @param Q The vector of Q-values corresponding to each operator.
 * @param iter The current iteration number, used for decaying epsilon if decayEpsilon_ is true.
 * @return The index of the selected operator in the operators_ vector.
 */
int VND_RL::selectOperator(const std::vector<double>& Q, int iter) {
  double eps = epsilon_;
  if (decayEpsilon_) {
    eps = epsilon_ * std::pow(0.99, iter);
    if (eps < 0.01) eps = 0.01;
  }
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  if (dist(rng_) < eps) {
    return std::uniform_int_distribution<int>(0, (int)Q.size() - 1)(rng_);
  } else {
    double maxQ = *std::max_element(Q.begin(), Q.end());
    std::vector<int> bestIdx;
    for (int i = 0; i < (int)Q.size(); ++i)
      if (std::abs(Q[i] - maxQ) < 1e-9) bestIdx.push_back(i);
    return bestIdx[std::uniform_int_distribution<int>(0, (int)bestIdx.size() - 1)(rng_)];
  }
}

/**
 * @brief Logs the current iteration number and the Q-values of all operators to a CSV file. This allows tracking the evolution of Q-values over time, 
 *        which can be useful for analyzing the learning process of the VND-RL algorithm.
 * @param iter The current iteration number.
 * @param Q The vector of Q-values corresponding to each operator at the current iteration.
 */
void VND_RL::logQValues(int iter, const std::vector<double>& Q) {
  if (!qlog_.is_open()) return;
  qlog_ << iter;
  for (double q : Q) qlog_ << "," << std::fixed << std::setprecision(4) << q;
  qlog_ << "\n";
}