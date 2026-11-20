// Facade for the API implementation

#include "../include/Sudoku.hpp"
#include "Solver.hpp"
#include "Board.hpp"

namespace sudoku
{
    namespace
    {
        int getTargetNodes(difficulty level)
        {
            switch (level)
            {
            case difficulty::EASY:
                return 1000;
            case difficulty::MID:
                return 5000;
            case difficulty::HARD:
                return 10000;
            default:
                return 0;
            }
        }
    }
    constexpr int ERROR_CODE = -2;
    constexpr int NO_SOLUTION_FOUND = -1;

    int Sudoku::solve(const std::string &board)
    {
        if (board.size() != 81)
            return ERROR_CODE;

        Board prepared(board);

        Solver solver;

        if (solver.solve(prepared))
            return 1;

        return NO_SOLUTION_FOUND;
    }

    int Sudoku::printSolution(const std::string &board)
    {
        if (board.size() != 81)
            return ERROR_CODE;

        Board prepared(board);

        Solver solver;

        if (solver.solve(prepared))
        {
            prepared.print();
            return 1;
        }

        return NO_SOLUTION_FOUND;
    }

    void Sudoku::generateBoard(difficulty difficulty)
    {
        int targetNodes = getTargetNodes(difficulty);

        Board generated;
        Generator generator;
        std::mt19937 seed;
        generator.generate(generated, seed, targetNodes);
        generated.print();
    }
    void Sudoku::generateBoard(difficulty difficulty, std::mt19937 &gen)
    {
        int targetNodes = getTargetNodes(difficulty);
        Board generated;
        Generator generator;
        generator.generate(generated, gen, targetNodes);
        generated.print();
    }
}