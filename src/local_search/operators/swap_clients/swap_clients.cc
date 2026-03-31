#include "./swap_clients.h"
#include <limits>

bool SwapClients::improve(Solution& sol, const Instance& inst) {
    bool anyImproved = false;

    bool foundMove = true;
    while (foundMove) {
        foundMove = false;

        double bestDelta = -EPS;
        int bestI1 = -1, bestI2 = -1, bestJ1 = -1, bestJ2 = -1;

        // Solo considerar clientes servidos íntegramente por una instalación
        for (int i1 = 0; i1 < inst.n; ++i1) {
            if ((int)sol.facilitiesOf[i1].size() != 1) continue;
            const int j1 = sol.facilitiesOf[i1][0];

            for (int i2 = i1 + 1; i2 < inst.n; ++i2) {
                if ((int)sol.facilitiesOf[i2].size() != 1) continue;
                const int j2 = sol.facilitiesOf[i2][0];
                if (j1 == j2) continue; // misma instalación, sin intercambio útil

                const double d1 = inst.demand[i1];
                const double d2 = inst.demand[i2];

                // ── Verificación de capacidad ────────────────────────────
                // j1 pierde d1 y gana d2 → necesita residualCap[j1] + d1 >= d2
                if (sol.residualCap[j1] + d1 < d2 - EPS) continue;
                // j2 pierde d2 y gana d1 → necesita residualCap[j2] + d2 >= d1
                if (sol.residualCap[j2] + d2 < d1 - EPS) continue;

                // ── Verificación de incompatibilidades ───────────────────
                // i1 pasa a j2: comprobar incompCount[i1][j2]
                //   Si i1 e i2 son incompatibles, i2 está saliendo de j2 → restar 1
                int incompI1inJ2 = sol.incompCount[i1][j2];
                if (inst.isIncompat[i1][i2]) --incompI1inJ2;
                if (incompI1inJ2 > 0) continue;

                // i2 pasa a j1: comprobar incompCount[i2][j1]
                //   Si i1 e i2 son incompatibles, i1 está saliendo de j1 → restar 1
                int incompI2inJ1 = sol.incompCount[i2][j1];
                if (inst.isIncompat[i1][i2]) --incompI2inJ1;
                if (incompI2inJ1 > 0) continue;

                // ── Variación de coste ───────────────────────────────────
                const double delta =
                    (inst.supplyCost[i1][j2] - inst.supplyCost[i1][j1]) * d1 +
                    (inst.supplyCost[i2][j1] - inst.supplyCost[i2][j2]) * d2;

                if (delta < bestDelta) {
                    bestDelta = delta;
                    bestI1 = i1; bestI2 = i2;
                    bestJ1 = j1; bestJ2 = j2;
                }
            }
        }

        // Aplicar el mejor intercambio encontrado
        if (bestI1 >= 0) {
            const double d1 = inst.demand[bestI1];
            const double d2 = inst.demand[bestI2];
            sol.removeAssignment(bestI1, bestJ1, d1, inst);
            sol.removeAssignment(bestI2, bestJ2, d2, inst);
            sol.assign(bestI1, bestJ2, d1, inst);
            sol.assign(bestI2, bestJ1, d2, inst);
            foundMove   = true;
            anyImproved = true;
        }
    }

    return anyImproved;
}