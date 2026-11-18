#pragma once

namespace minesweeper {

class Cell {
public:
    Cell() = default;

    bool HasMine() const noexcept { return has_mine_; }
    void SetMine(bool val) noexcept { has_mine_ = val; }

    bool IsRevealed() const noexcept { return is_revealed_; }
    void Reveal() noexcept { is_revealed_ = true; }

    bool IsFlagged() const noexcept { return is_flagged_; }
    void ToggleFlag() noexcept { is_flagged_ = !is_flagged_; }

    int NeighborMines() const noexcept { return neighbor_mines_; }
    void SetNeighborMines(int count) noexcept { neighbor_mines_ = count; }

    // Возвращает символ клетки:
    // '.' - закрыта, 'F' - флаг, '*' - мина, ' ' - пусто, '1'..'8' - количество мин
    char GetDisplayChar(bool revealMines = false) const noexcept;

private:
    bool has_mine_ = false;
    bool is_revealed_ = false;
    bool is_flagged_ = false;
    int neighbor_mines_ = 0;
};

} // namespace minesweeper
