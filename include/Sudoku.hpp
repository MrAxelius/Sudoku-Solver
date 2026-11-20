#pragma once

#include <random>
#include <string>

namespace sudoku
{
    // As the boards are randomly generated, mid and hard may not happen on the firsr
    // try, take that into consideration before useing the boards
    enum class difficulty
    {
        EASY, // about 1000 nodes in search to the solveing algo
        MID,  // about 5000 nodes
        HARD  // about 10000 nodes
    };
    class Sudoku
    {
    public:
        int solve(const std::string &board);
        int printSolution(const std::string &board);

        void generateBoard(difficulty difficulty);
        void generateBoard(difficulty difficulty, std::mt19937 &gen);
    };
} // namespace sudoku
