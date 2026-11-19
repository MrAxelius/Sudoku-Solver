#include <cstdint>
#include <iostream>
#include <array>
#include <algorithm>

using cell = std::uint8_t;
using mask = std::uint16_t;
constexpr mask full = 0x3FE;
constexpr mask empty = 0;

struct emptyCounter
{
    std::uint32_t hi = 0; // 0 -- 63
    std::uint64_t lo = 0; // 64 -- 80
};

struct Board
{
    std::array<cell, 81> cells{};
    emptyCounter emptyIndex;

    // Masks
    mask column_[9]{};
    mask row_[9]{};
    mask subSquare_[9]{};

    Board(std::string sudoku)
    {
        for (int i = 0; i < 81; ++i)
        {
            if (sudoku[i] == '0')
            {
                if (i < 64)
                {
                    emptyIndex.lo |= (1ull << i);
                }
                else
                {
                    emptyIndex.hi |= (1u << (i - 64));
                }
                continue;
            }

            cells[i] = static_cast<int>(sudoku[i]) - '0';
            cell value = cells[i];

            std::uint16_t row = i / 9;
            std::uint16_t col = i % 9;
            std::uint16_t subSquare = (row / 3) * 3 + (col / 3);

            mask bit = 1u << value;

            row_[row] |= bit;
            column_[col] |= bit;
            subSquare_[subSquare] |= bit;
        }
    }

    void print() const
    {
        int8_t count = 0;
        for (auto cell : cells)
        {
            ++count;
            std::cout << cell << '|';
            if (count % 9 == 0)
            {
                count = 0;
                std::cout << '\n';
            }
        }
    }

    bool checkEmpty(cell cellNumber) const
    {
        return cells[cellNumber] == 0;
    }

    bool clearCell(cell cellNumber)
    {

        int old = cells[cellNumber];
        cells[cellNumber] = 0;
        auto masks = findRelevantMasks(cellNumber);

        for (auto &mask : masks)
        {
            *mask &= ~(1u << old);
        }
        if (cellNumber < 64)
        {
            emptyIndex.lo |= (1ull << cellNumber);
        }
        else
        {
            emptyIndex.hi |= (1u << (cellNumber - 64));
        }
        return true;
    }

    bool modifyCell(cell cellNumber, cell newNumber)
    {
        if (cells[cellNumber] == newNumber)
            return true;

        // Where is the cell
        std::uint16_t row = cellNumber / 9;
        std::uint16_t col = cellNumber % 9;
        std::uint16_t subSquare = (row / 3) * 3 + (col / 3);

        mask bit = 1u << newNumber;
        // if actual number is 0, no negation required
        if (checkEmpty(cellNumber))
        {
            row_[row] |= bit;
            column_[col] |= bit;
            subSquare_[subSquare] |= bit;
            cells[cellNumber] = newNumber;

            if (cellNumber < 64)
            {
                emptyIndex.lo &= ~(1ULL << cellNumber);
            }
            else
            {
                emptyIndex.hi &= ~(1ULL << (cellNumber - 64));
            }

            return true;
        }

        if ((row_[row] & bit) ||
            (column_[col] & bit) ||
            (subSquare_[subSquare] & bit))
        {
            return false;
        }

        // Turn off the old number
        mask old = 1u << cells[cellNumber];
        row_[row] &= ~old;
        column_[col] &= ~old;
        subSquare_[subSquare] &= ~old;

        // Turn on the new number
        row_[row] |= bit;
        column_[col] |= bit;
        subSquare_[subSquare] |= bit;

        cells[cellNumber] = newNumber;

        if (cellNumber < 64)
            emptyIndex.lo &= ~(1ULL << cellNumber);
        else
            emptyIndex.hi &= ~(1ULL << (cellNumber - 64));

        return true;
    }

    std::array<mask *, 3> findRelevantMasks(cell cellNumber)
    {
        std::int_fast8_t column = cellNumber % 9;
        std::int_fast8_t row = cellNumber / 9;
        return {&row_[row], &column_[column], &subSquare_[(row) / 3 * 3 + ((column) / 3)]};
    }

    bool isBoardComplete()
    {
        for (int i = 0; i <= 8; ++i)
        {
            if (row_[i] != full || column_[i] != full || subSquare_[i] != full)
            {
                return false;
            }
        }
        return true;
    }
};

/*
0  1  2  3  4  5  6  7  8
9  10 11 12 13 14 15 16 17
18 19 20 21 22 23 24 25 26
27 28 29 30 31 32 33 34 35
36 37 38 39 40 41 42 43 44
45 46 47 48 49 50 51 52 53
54 55 56 57 58 59 60 61 62
63 64 65 66 67 68 69 70 71
72 73 74 75 76 77 78 79 80
*/
