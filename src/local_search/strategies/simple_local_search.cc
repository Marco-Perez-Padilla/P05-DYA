#include "simple_local_search.h"

bool SimpleLocalSearch::improve(Solution& sol, const Instance& inst) {
    bool anyImproved = false;
    bool improved = true;

    // Repetir hasta alcanzar óptimo local
    while (improved) {
        improved = false;

        for (auto& op : operators_) {
            if (op->improve(sol, inst)) {
                improved = true;
                anyImproved = true;
            }
        }
    }

    return anyImproved;
}