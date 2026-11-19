#include "../src/Solver.cpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <random>
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
    uint64_t checksum;
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

// Genera 'inner' veces un puzzle con los huecos indicados.
static Result runOnce(int targetHoles, int inner, std::mt19937 &gen)
{
    double total = 0.0;
    bool allOk = true;
    bool stable = true;
    uint64_t nodes = 0;
    uint64_t totalChecksum = 0;
    
    // Tablero vacío inicial (81 ceros)
    const std::string emptyPuzzle(81, '0');

    for (int i = 0; i < inner; ++i)
    {
        Board board(emptyPuzzle);
        Generator generator;

        // EL ENGAÑO: Obligamos al compilador a no optimizar el tablero
        escape(&board);

        const auto t0 = std::chrono::steady_clock::now();
        
        // 1. Llenar el tablero
        generator.fillBoard(board, gen);
        // 2. Excavar
        generator.dig(board, gen, targetHoles);
        
        const auto t1 = std::chrono::steady_clock::now();

        total += std::chrono::duration<double, std::milli>(t1 - t0).count();
        
        // Consumimos el resultado para que no lo borre
        totalChecksum += board.cells[0]; 
        totalChecksum += generator.counter; // Nodos visitados por el generador

        // Guardamos los nodos de la primera iteración para hacer la media
        if (i == 0)
            nodes = generator.counter;
        else
            stable &= (generator.counter == nodes);
    }

    return {
        total / inner,
        allOk,
        nodes,
        stable,
        totalChecksum
    };
}

static Statistics calculateStatistics(std::vector<double> values)
{
    std::sort(values.begin(), values.end());
    const size_t count = values.size();
    const double min = values.front();
    const double max = values.back();
    double median;

    if (count % 2 == 0) median = (values[count / 2 - 1] + values[count / 2]) / 2.0;
    else median = values[count / 2];

    double sum = 0.0;
    for (double value : values) sum += value;
    const double mean = sum / count;

    double squaredDifferences = 0.0;
    for (double value : values)
    {
        const double difference = value - mean;
        squaredDifferences += difference * difference;
    }

    const double stddev = count > 1 ? std::sqrt(squaredDifferences / (count - 1)) : 0.0;
    const size_t p90Index = static_cast<size_t>(std::ceil(0.90 * count)) - 1;
    const double p90 = values[p90Index];

    return {min, median, max, mean, p90, stddev};
}

int main()
{
    struct Case
    {
        const char *name;
        int targetHoles;
        int inner;
    };

    constexpr int warmup = 2;
    // Hacemos menos repeticiones porque generar es MUCHO más lento que resolver
    constexpr int reps = 20; 
    
    // Resoluciones por muestra (cuántos puzzles generamos por cada iteración)
    constexpr int resolutionsPerSample = 10;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<Case> cases = {
        {"easy", 30, resolutionsPerSample},
        {"hard", 50, resolutionsPerSample}};

    for (const auto &c : cases)
    {
        // Warmup
        for (int i = 0; i < warmup; ++i)
            runOnce(c.targetHoles, c.inner, gen);

        std::vector<double> times;
        times.reserve(reps);

        bool allOk = true;
        bool stable = true;
        uint64_t nodes = 0;
        uint64_t finalChecksum = 0;

        // Mediciones
        for (int i = 0; i < reps; ++i)
        {
            const Result r = runOnce(c.targetHoles, c.inner, gen);

            times.push_back(r.ms);
            allOk &= r.solved;
            stable &= r.nodesStable;
            finalChecksum += r.checksum;

            if (i == 0)
                nodes = r.nodes;
            else
                stable &= (r.nodes == nodes);
        }

        const Statistics stats = calculateStatistics(times);

        // Nodos visitados por el generador (búsquedas de unicidad)
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
            "         nodos(gen) %llu  %.1f ns/nodo  %.2f Mnodos/s  %s\n",
            static_cast<unsigned long long>(nodes),
            nsPerNode,
            nodesPerSec / 1e6,
            stable ? "(nodos estables)" : "(NODOS VARIABLES)");

        std::printf(
            "         muestras %d  puzzles/muestra %d (total %d generados)\n",
            reps,
            c.inner,
            reps * c.inner);
            
        std::printf(
            "         checksum %llu (ignorar)\n",
            static_cast<unsigned long long>(finalChecksum));

        std::printf(
            "         media %.4f ms  P90 %.4f ms  desviacion %.4f ms\n\n",
            stats.mean,
            stats.p90,
            stats.stddev);
    }

    return 0;
}