#include "greedy_constructive.h"
#include <numeric>
#include <algorithm>

Solution GreedyConstructive::build(const Instance& inst) {
    Solution sol = Solution::createEmpty(inst);
    const int m = inst.m, n = inst.n;

    // ── FASE 1: Selección de instalaciones ───────────────────────────────
    // Ordenar índices de instalaciones por coste fijo ascendente
    std::vector<int> order(m);
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(),
              [&](int a, int b){ return inst.fixedCost[a] < inst.fixedCost[b]; });

    double totalDemand = 0.0;
    for (double d : inst.demand) totalDemand += d;

    double accCap = 0.0;
    int idx = 0;

    // Abrir instalaciones hasta cubrir la demanda total
    while (idx < m && accCap < totalDemand) {
        const int j = order[idx++];
        sol.openFacility(j, inst);
        accCap += inst.capacity[j];
    }

    // Añadir k instalaciones extra por holgura de incompatibilidad
    for (int extra = 0; extra < k_ && idx < m; ++extra, ++idx)
        sol.openFacility(order[idx], inst);

    // ── FASE 2: Asignación de clientes ───────────────────────────────────
    for (int i = 0; i < n; ++i) {
        // Ordenar instalaciones abiertas por coste de transporte para el cliente i
        std::vector<int> facOrder;
        facOrder.reserve(m);
        for (int j = 0; j < m; ++j)
            if (sol.open[j]) facOrder.push_back(j);
        std::sort(facOrder.begin(), facOrder.end(),
                  [&](int a, int b){ return inst.supplyCost[i][a] < inst.supplyCost[i][b]; });

        double remaining = inst.demand[i];
        for (int j : facOrder) {
            if (remaining < EPS) break;

            // Verificar compatibilidad: ningún incompatible de i está en j
            if (sol.incompCount[i][j] > 0) continue;

            // Verificar capacidad residual
            if (sol.residualCap[j] < EPS) continue;

            const double q = std::min(remaining, sol.residualCap[j]);
            sol.assign(i, j, q, inst);
            remaining -= q;
        }
        // Si remaining > EPS, la demanda no se satisfizo (solución infactible)
    }

    return sol;
}