#include "Cell.hpp"

namespace minesweeper {

char Cell::GetDisplayChar(bool revealMines) const noexcept {
    if (is_flagged_) {
        return 'F';
    }
    if (!is_revealed_) {
        if (revealMines && has_mine_) {
            return '*';
        }
        return '.';
    }
    if (has_mine_) {
        return '*';
    }
    if (neighbor_mines_ == 0) {
        return ' ';
    }
    return static_cast<char>('0' + neighbor_mines_);
}

} // namespace minesweeper
