#pragma once

#include "Cell.hpp"
#include <vector>

namespace minesweeper {

class Board {
public:
    Board(int width, int height, int total_mines);

    int GetWidth() const noexcept { return width_; }
    int GetHeight() const noexcept { return height_; }
    int GetTotalMines() const noexcept { return total_mines_; }

    const Cell& GetCell(int x, int y) const;
    Cell& GetCell(int x, int y);

    bool IsValidCoordinate(int x, int y) const noexcept;

    // Инициализация поля (первый ход игрока гарантированно безопасен)
    void Initialize(int firstX, int firstY);
    bool IsInitialized() const noexcept { return is_initialized_; }

    // Открыть клетку. Возвращает false, если игрок подорвался на мине
    bool Reveal(int x, int y);

    // Установить или снять флаг
    void ToggleFlag(int x, int y);

    // Проверка победы (все клетки без мин успешно открыты)
    bool CheckWinCondition() const noexcept;

    // Вывод поля в консоль
    void Print(bool revealMines = false) const;

private:
    int width_;
    int height_;
    int total_mines_;
    bool is_initialized_ = false;
    std::vector<Cell> cells_;

    int ToIndex(int x, int y) const noexcept {
        return y * width_ + x;
    }

    void CountNeighbors();
    void FloodFillEmptyCells(int startX, int startY);
};

} // namespace minesweeper
