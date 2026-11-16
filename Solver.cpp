#include "Board.cpp"

#include <cstdint>
#include <algorithm>

struct Solver
{
    bool solve(Board &board)
    {
        int index = findEmptyCell(board);
        if (index == -1)
        {
            return board.isBoardComplete();
        }
        for (std::int_fast8_t i = 1; i <= 9; ++i)
        {
            if (canPlaceNumber(index, i, board))
            {
                board.modifyCell(index, i);
                if(solve(board)){
                    return true;
                }
                board.clearCell(index);
            }
        }
        return false;
    }

    int findEmptyCell(Board &board) const
    {
        for (std::size_t i = 0; i < board.cells.size(); ++i)
            if (board.cells[i] == 0)
                return static_cast<int>(i);
        return -1;
    }

    bool canPlaceNumber(int cellIndex, cell number, Board &board)
    {
        const mask bit = static_cast<mask>(1u << number);
        auto masks = board.findRelevantMasks(cellIndex);

        return std::ranges::none_of(masks, [bit](mask *m)
                                    { return *m & bit; });
    }
};