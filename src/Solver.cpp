#include "Board.cpp"

#include <cstdint>
#include <algorithm>
#include <bit>

using cell = std::uint8_t;
using mask = std::uint16_t;

struct Solver
{
    uint64_t counter = 0;
    bool solve(Board &board)
    {
        counter++;
        auto emptyCell = findEmptyCell(board);
        int index = emptyCell.first;
        mask mask_ = emptyCell.second;
        if (index == -2)
        {
            return false;
        }
        if (index == -1)
        {
            return board.isBoardComplete();
        }
        mask lowestBit;
        while (mask_ != 0)
        {
            lowestBit = mask_ & (-mask_);
            auto number = std::countr_zero(lowestBit);
            board.modifyCell(index, number);
            if (solve(board))
            {
                return true;
            }
            board.clearCell(index);
            mask_ = mask_ & (mask_ - 1);
        }
        return false;
    }

    std::pair<int, mask> findEmptyCell(Board &board) const
    {
        int minCandidates = 10;
        int bestCell = -1;
        for (std::int_fast8_t i = 0; i <= 80; ++i)
        {
            if (!board.checkEmpty(i))
            {
                continue;
            }
            auto masks = board.findRelevantMasks(i);
            mask candidates = full & ~(*masks[0] | *masks[1] | *masks[2]);
            uint8_t numberBits = std::popcount(candidates);
            if (candidates == 0)
            {
                return {-2, 0};
            }
            if (numberBits == 1)
            {
                return {i, candidates};
            }
            if (numberBits < minCandidates && numberBits != 0)
            {
                minCandidates = numberBits;
                bestCell = i;
            }
        }
        if (bestCell == -1)
            return {-1, 0};

        auto masks = board.findRelevantMasks(bestCell);
        mask candidates = full & ~(*masks[0] | *masks[1] | *masks[2]);
        return {bestCell, candidates};
    }

    bool canPlaceNumber(int cellIndex, cell number, Board &board)
    {
        const mask bit = static_cast<mask>(1u << number);
        auto masks = board.findRelevantMasks(cellIndex);

        return std::ranges::none_of(masks, [bit](mask *m)
                                    { return *m & bit; });
    }
};