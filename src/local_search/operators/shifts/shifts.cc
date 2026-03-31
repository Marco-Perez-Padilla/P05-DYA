#include "./shifts.h"
#include <limits>
#include <algorithm>

bool Shifts::improve(Solution& sol, const Instance& inst) {
    bool anyImproved = false;

    // Repetir hasta que no haya movimientos de mejora
    bool foundMove = true;
    while (foundMove) {
        foundMove = false;

        double bestDelta = -EPS; // solo aceptamos mejoras estrictas
        int    bestI = -1, bestJ1 = -1, bestJ2 = -1;
        double bestQ = 0.0;

        // Recorrer todos los clientes
        for (int i = 0; i < inst.n; ++i) {
            // j1: instalaciones que sirven actualmente a i
            for (int j1 : sol.facilitiesOf[i]) {
                const double amountAtJ1 = sol.x[i][j1] * inst.demand[i];
                if (amountAtJ1 < EPS) continue;

                // j2: otras instalaciones abiertas
                for (int j2 = 0; j2 < inst.m; ++j2) {
                    if (!sol.open[j2] || j2 == j1) continue;

                    // Solo merece explorar si j2 es más barato que j1 para i
                    if (inst.supplyCost[i][j2] >= inst.supplyCost[i][j1]) continue;

                    // Verificar compatibilidad en j2:
                    //   - Si i ya está en j2 (w[i][j2]=true): compatible por definición
                    //   - Si no: ningún incompatible de i debe estar en j2
                    if (!sol.w[i][j2] && sol.incompCount[i][j2] > 0) continue;

                    // Verificar capacidad residual en j2
                    if (sol.residualCap[j2] < EPS) continue;

                    // Cantidad máxima transferible
                    const double q = std::min(amountAtJ1, sol.residualCap[j2]);
                    if (q < EPS) continue;

                    // Variación de coste de transporte (negativa = mejora)
                    const double delta = (inst.supplyCost[i][j2] - inst.supplyCost[i][j1]) * q;

                    if (delta < bestDelta) {
                        bestDelta = delta;
                        bestI = i; bestJ1 = j1; bestJ2 = j2;
                        bestQ = q;
                    }
                }
            }
        }

        // Aplicar el mejor movimiento encontrado
        if (bestI >= 0) {
            sol.removeAssignment(bestI, bestJ1, bestQ, inst);
            sol.assign(bestI, bestJ2, bestQ, inst);
            foundMove    = true;
            anyImproved  = true;
        }
    }

    return anyImproved;
}