
#include <iostream>
#include <string>
#include <random>
#include "Sudoku.hpp"

int main()
{
    sudoku::Sudoku sudoku;

    // A valid Sudoku with a known solution
    const std::string validBoard =
        "530070000"
        "600195000"
        "098000060"
        "800060003"
        "400803001"
        "700020006"
        "060000280"
        "000419005"
        "000080079";

    // A board with an invalid length
    const std::string invalidLength =
        "530070000";

    // A board with 81 characters but contradictory values
    const std::string unsolvableBoard =
        "550070000"
        "600195000"
        "098000060"
        "800060003"
        "400803001"
        "700020006"
        "060000280"
        "000419005"
        "000080079";

    std::cout << "API tests\n\n";

    std::cout << "[1] solve() - valid board\n";
    std::cout << "Result: " << sudoku.solve(validBoard) << "\n\n";

    std::cout << "[2] printSolution() - valid board\n";
    std::cout << "Result: " << sudoku.printSolution(validBoard) << "\n\n";

    std::cout << "[3] solve() - invalid length\n";
    std::cout << "Result: " << sudoku.solve(invalidLength) << "\n\n";

    std::cout << "[4] solve() - unsolvable board\n";
    std::cout << "Result: " << sudoku.solve(unsolvableBoard) << "\n\n";

    std::cout << "[5] generateBoard() - EASY\n";
    sudoku.generateBoard(sudoku::difficulty::EASY);
    std::cout << "\n";

    std::cout << "[6] generateBoard() - HARD with custom seed\n";
    std::mt19937 gen(42);
    sudoku.generateBoard(sudoku::difficulty::HARD, gen);

    std::cout << "\n===== TESTS FINISHED =====\n";

    return 0;
}
