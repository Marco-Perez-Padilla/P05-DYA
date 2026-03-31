#include "grasp_constructive.h"
#include <numeric>
#include <algorithm>

Solution GRASPConstructive::build(const Instance& inst) {
    Solution sol = Solution::createEmpty(inst);
    const int m = inst.m, n = inst.n;

    // ── FASE 1: Selección de instalaciones (aleatorizada) ────────────────
    // Ordenar instalaciones por coste fijo ascendente
    std::vector<int> order(m);
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(),
              [&](int a, int b){ return inst.fixedCost[a] < inst.fixedCost[b]; });

    double totalDemand = 0.0;
    for (double d : inst.demand) totalDemand += d;

    double accCap = 0.0;
    int idx = 0;

    // Abrir instalaciones eligiendo aleatoriamente de la LRC
    while (idx < m && accCap < totalDemand) {
        const int rclSize = std::min(alpha_, m - idx);
        std::uniform_int_distribution<int> pick(0, rclSize - 1);
        const int chosen = order[idx + pick(rng_)];

        // Llevar el elegido a la posición idx (intercambio para no repetir)
        auto it = std::find(order.begin() + idx,
                            order.begin() + idx + rclSize, chosen);
        std::iter_swap(it, order.begin() + idx);

        sol.openFacility(chosen, inst);
        accCap += inst.capacity[chosen];
        ++idx;
    }

    // Añadir k instalaciones extra de holgura (también con LRC)
    for (int extra = 0; extra < k_ && idx < m; ++extra) {
        const int rclSize = std::min(alpha_, m - idx);
        std::uniform_int_distribution<int> pick(0, rclSize - 1);
        const int chosen = order[idx + pick(rng_)];

        auto it = std::find(order.begin() + idx,
                            order.begin() + idx + rclSize, chosen);
        std::iter_swap(it, order.begin() + idx);

        sol.openFacility(chosen, inst);
        ++idx;
    }

    // ── FASE 2: Asignación de clientes (orden y LRC aleatorizados) ───────
    std::vector<int> clientOrder(n);
    std::iota(clientOrder.begin(), clientOrder.end(), 0);
    std::shuffle(clientOrder.begin(), clientOrder.end(), rng_);

    for (int i : clientOrder) {
        // Obtener instalaciones abiertas ordenadas por coste para el cliente i
        std::vector<int> facOrder;
        facOrder.reserve(m);
        for (int j = 0; j < m; ++j)
            if (sol.open[j]) facOrder.push_back(j);
        std::sort(facOrder.begin(), facOrder.end(),
                  [&](int a, int b){ return inst.supplyCost[i][a] < inst.supplyCost[i][b]; });

        double remaining = inst.demand[i];
        while (remaining > EPS) {
            // Construir LRC: las alpha mejores instalaciones compatibles con capacidad
            std::vector<int> rcl;
            rcl.reserve(alpha_);
            for (int j : facOrder) {
                if (sol.incompCount[i][j] > 0) continue; // incompatible
                if (sol.residualCap[j] < EPS)  continue; // sin capacidad
                rcl.push_back(j);
                if ((int)rcl.size() >= alpha_) break;
            }
            if (rcl.empty()) break; // no se puede satisfacer más demanda

            // Elegir aleatoriamente de la LRC
            std::uniform_int_distribution<int> pick(0, (int)rcl.size() - 1);
            const int j = rcl[pick(rng_)];

            const double q = std::min(remaining, sol.residualCap[j]);
            sol.assign(i, j, q, inst);
            remaining -= q;
        }
    }

    return sol;
}