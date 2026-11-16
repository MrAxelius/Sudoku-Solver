#include "src/Solver.cpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

struct Result { double ms; bool solved; uint64_t nodes; bool nodesStable; };

// Resuelve 'inner' veces el mismo puzzle y devuelve el tiempo medio por resolución.
// Cada resolución parte de un Board y un Solver nuevos, fuera del cronómetro.
static Result runOnce(const std::string &puzzle, int inner)
{
    double total = 0.0;
    bool allOk = true;
    bool stable = true;
    uint64_t nodes = 0;

    for (int i = 0; i < inner; ++i)
    {
        Board board(puzzle);
        Solver solver;

        auto t0 = std::chrono::steady_clock::now();
        bool ok = solver.solve(board);
        auto t1 = std::chrono::steady_clock::now();

        total += std::chrono::duration<double, std::milli>(t1 - t0).count();
        allOk &= ok;

        // Los nodos son deterministas: todas las resoluciones deben dar lo mismo
        if (i == 0) nodes = solver.counter;
        else stable &= (solver.counter == nodes);
    }
    return { total / inner, allOk, nodes, stable };
}

int main()
{
    struct Case { const char *name; std::string puzzle; int inner; };

    std::vector<Case> cases = {
        { "hard", "800000000003600000070090200050007000000045700000100030001000068008500010090000400", 1 },
        { "easy",   "004300209005009001070060043006002087190007400050083000600000105003508690042910300", 1000 }
    };

    constexpr int warmup = 3;
    constexpr int reps   = 20;

    for (const auto &c : cases)
    {
        if (c.puzzle.size() != 81)
        {
            std::printf("%s: longitud %zu, se esperaban 81\n", c.name, c.puzzle.size());
            continue;
        }

        for (int i = 0; i < warmup; ++i) runOnce(c.puzzle, c.inner);

        std::vector<double> times;
        bool allOk = true;
        bool stable = true;
        uint64_t nodes = 0;

        for (int i = 0; i < reps; ++i)
        {
            auto r = runOnce(c.puzzle, c.inner);
            times.push_back(r.ms);
            allOk &= r.solved;
            stable &= r.nodesStable;
            if (i == 0) nodes = r.nodes;
            else stable &= (r.nodes == nodes);
        }

        std::sort(times.begin(), times.end());
        double med = times[reps / 2];
        double nsPerNode = nodes ? med * 1e6 / nodes : 0.0;
        double nodesPerSec = med > 0 ? nodes / (med / 1000.0) : 0.0;

        std::printf("%-8s min %.4f ms  mediana %.4f ms  max %.4f ms  %s\n",
                    c.name, times.front(), med, times.back(),
                    allOk ? "OK" : "FALLO");
        std::printf("         nodos %llu  %.1f ns/nodo  %.2f Mnodos/s  %s\n",
                    static_cast<unsigned long long>(nodes),
                    nsPerNode, nodesPerSec / 1e6,
                    stable ? "(nodos estables)" : "(NODOS VARIABLES: estado contaminado)");
    }
}