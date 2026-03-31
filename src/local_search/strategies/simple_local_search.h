#pragma once
#include "../local_search.h"
#include <vector>
#include <memory>

/**
 * @brief Estrategia simple de búsqueda local:
 * aplica varias búsquedas locales en secuencia hasta no mejorar.
 *
 * Equivalente a un VND básico (sin cambio dinámico de orden).
 */
class SimpleLocalSearch : public LocalSearch {
public:
    SimpleLocalSearch(std::vector<std::shared_ptr<LocalSearch>> operators)
        : operators_(std::move(operators)) {}

    bool improve(Solution& sol, const Instance& inst) override;

    std::string name() const override { return "SimpleLocalSearch"; }

private:
    std::vector<std::shared_ptr<LocalSearch>> operators_;
};