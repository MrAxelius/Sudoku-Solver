#include "Board.cpp"

#include <cstdint>
#include <algorithm>
#include <bit>

using cell = std::uint8_t;
using mask = std::uint16_t;

struct Solver
{
    // benchmarking tool
    uint64_t counter = 0;
    emptyCounter index;
    bool solve(Board &board)
    {
        ++counter;
        auto emptyCell = findBestCell(board);
        int index = emptyCell.first;
        mask mask_ = emptyCell.second;
        if (index == -2)
        {
            return false;
        }
        if (index == -1)
        {
            return true;
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

    std::pair<int, mask> findBestCell(Board &board)
    {
        int minCandidates = 10;
        int bestCell = -1;
        mask bestMask = 0;
        auto emptyHi = board.emptyIndex.hi;
        auto emptyLo = board.emptyIndex.lo;
        mask candidates = 0;
        int index = -1;

        if (!emptyHi && !emptyLo)
        {
            return {-1, 0};
        }

        while (emptyHi != 0 || emptyLo != 0)
        {
            if (emptyLo != 0)
            {
                index = std::countr_zero(emptyLo);
                auto masks = board.findRelevantMasks(index);
                candidates = full & ~(*masks[0] | *masks[1] | *masks[2]);
                std::uint8_t numCandidates = std::popcount(candidates);

                if (numCandidates == 0)
                {
                    return {-2, 0};
                }
                if (numCandidates == 1)
                {
                    return {index, candidates};
                }
                if (numCandidates < minCandidates)
                {
                    minCandidates = numCandidates;
                    bestCell = index;
                    bestMask = candidates;
                }
                emptyLo &= emptyLo - 1;
            }
            else if (emptyHi != 0)
            {
                index = std::countr_zero(emptyHi) + 64;
                auto masks = board.findRelevantMasks(index);
                candidates = full & ~(*masks[0] | *masks[1] | *masks[2]);
                std::uint8_t numCandidates = std::popcount(candidates);

                if (numCandidates == 0)
                {
                    return {-2, 0};
                }
                if (numCandidates == 1)
                {
                    return {index, candidates};
                }
                if (numCandidates < minCandidates)
                {
                    minCandidates = numCandidates;
                    bestCell = index;
                    bestMask = candidates;
                }
                emptyHi &= emptyHi - 1;
            }
        }

        if (bestCell == -1)
            return {-1, 0};

        return {bestCell, bestMask};
    }
};