#include "../src/Solver.cpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

template <class T>
inline void escape(T *p)
{
    asm volatile("" : : "g"(p) : "memory");
}

struct Result
{
    double ms;
    bool solved;
    uint64_t nodes;
    bool nodesStable;
    uint64_t checksum; // <-- NUEVO: Para forzar al compilador a ejecutar el solver
};

struct Statistics
{
    double min;
    double median;
    double max;
    double mean;
    double p90;
    double stddev;
};


static Result runOnce(const std::string &puzzle, int inner)
{
    double total = 0.0;
    bool allOk = true;
    bool stable = true;
    uint64_t nodes = 0;
    uint64_t totalChecksum = 0;

    for (int i = 0; i < inner; ++i)
    {
        Board board(puzzle);
        Solver solver;

        // EL ENGAÑO: Le decimos al compilador que el tablero puede cambiar
        escape(&board);

        const auto t0 = std::chrono::steady_clock::now();
        const bool ok = solver.solve(board);
        const auto t1 = std::chrono::steady_clock::now();

        total += std::chrono::duration<double, std::milli>(t1 - t0).count();
        allOk &= ok;

        // Consumimos los resultados para que el compilador no los ignore
        totalChecksum += solver.counter;
        totalChecksum += board.cells[0]; // Asumiendo que tienes un getter, o usa cells[0] si es público

        if (i == 0)
            nodes = solver.counter;
        else
            stable &= (solver.counter == nodes);
    }

    return {
        total / inner,
        allOk,
        nodes,
        stable,
        totalChecksum // <-- NUEVO
    };
}

static Statistics calculateStatistics(std::vector<double> values)
{
    std::sort(values.begin(), values.end());

    const size_t count = values.size();

    const double min = values.front();
    const double max = values.back();

    double median;

    if (count % 2 == 0)
    {
        median = (values[count / 2 - 1] + values[count / 2]) / 2.0;
    }
    else
    {
        median = values[count / 2];
    }

    double sum = 0.0;

    for (double value : values)
        sum += value;

    const double mean = sum / count;

    double squaredDifferences = 0.0;

    for (double value : values)
    {
        const double difference = value - mean;
        squaredDifferences += difference * difference;
    }

    const double stddev = count > 1
                              ? std::sqrt(squaredDifferences / (count - 1))
                              : 0.0;

    // Percentil 90 mediante nearest-rank.
    const size_t p90Index = static_cast<size_t>(std::ceil(0.90 * count)) - 1;

    const double p90 = values[p90Index];

    return {
        min,
        median,
        max,
        mean,
        p90,
        stddev};
}

int main()
{
    struct Case
    {
        const char *name;
        std::string puzzle;
        int inner;
    };

    constexpr int warmup = 3;
    constexpr int reps = 50;
    // ¡Ojo aquí! Si esto sigue siendo muy rápido, súbelo a 100000 o 500000
    constexpr int resolutionsPerSample = 10000;

    std::vector<Case> cases = {
        {"hard",
         "800000000003600000070090200050007000000045700000100030001000068008500010090000400",
         resolutionsPerSample},
        {"easy",
         "004300209005009001070060043006002087190007400050083000600000105003508690042910300",
         resolutionsPerSample}};

    for (const auto &c : cases)
    {
        if (c.puzzle.size() != 81)
        {
            std::printf("%s: longitud %zu, se esperaban 81\n", c.name, c.puzzle.size());
            continue;
        }

        // Warmup
        for (int i = 0; i < warmup; ++i)
            runOnce(c.puzzle, c.inner);

        std::vector<double> times;
        times.reserve(reps);

        bool allOk = true;
        bool stable = true;
        uint64_t nodes = 0;
        uint64_t finalChecksum = 0;

        // Mediciones
        for (int i = 0; i < reps; ++i)
        {
            const Result r = runOnce(c.puzzle, c.inner);

            times.push_back(r.ms);
            allOk &= r.solved;
            stable &= r.nodesStable;
            finalChecksum += r.checksum; // <-- Acumulamos el checksum

            if (i == 0)
                nodes = r.nodes;
            else
                stable &= (r.nodes == nodes);
        }

        const Statistics stats = calculateStatistics(times);

        // Tiempo medio por resolución.
        const double nsPerNode = nodes
                                     ? stats.median * 1e6 / nodes
                                     : 0.0;

        const double nodesPerSec = stats.median > 0.0
                                       ? nodes / (stats.median / 1000.0)
                                       : 0.0;

        std::printf(
            "%-8s min %.4f ms  mediana %.4f ms  max %.4f ms  %s\n",
            c.name,
            stats.min,
            stats.median,
            stats.max,
            allOk ? "OK" : "FALLO");

        std::printf(
            "         nodos %llu  %.1f ns/nodo  %.2f Mnodos/s  %s\n",
            static_cast<unsigned long long>(nodes),
            nsPerNode,
            nodesPerSec / 1e6,
            stable ? "(nodos estables)" : "(NODOS VARIABLES: estado contaminado)");

        std::printf(
            "         muestras %d  resoluciones/muestra %d\n",
            reps,
            c.inner);
            
        std::printf(
            "         checksum %llu (ignorar)\n",
            static_cast<unsigned long long>(finalChecksum));

        std::printf(
            "         media %.4f ms  P90 %.4f ms  desviacion %.4f ms\n",
            stats.mean,
            stats.p90,
            stats.stddev);
    }

    return 0;
}