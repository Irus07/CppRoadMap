#include "Board.hpp"
#include <iostream>
#include <random>
#include <queue>
#include <algorithm>

namespace minesweeper {

Board::Board(int width, int height, int total_mines)
    : width_(width), height_(height), total_mines_(total_mines),
      cells_(static_cast<size_t>(width * height)) {
    // Ограничиваем количество мин размером поля
    if (total_mines_ >= width_ * height_) {
        total_mines_ = (width_ * height_) - 1;
    }
}

bool Board::IsValidCoordinate(int x, int y) const noexcept {
    return x >= 0 && x < width_ && y >= 0 && y < height_;
}

const Cell& Board::GetCell(int x, int y) const {
    return cells_[static_cast<size_t>(ToIndex(x, y))];
}

Cell& Board::GetCell(int x, int y) {
    return cells_[static_cast<size_t>(ToIndex(x, y))];
}

void Board::Initialize(int firstX, int firstY) {
    if (is_initialized_) return;

    // Собираем все координаты, кроме клетки первого хода игрока
    std::vector<std::pair<int, int>> availableCoords;
    availableCoords.reserve(static_cast<size_t>(width_ * height_));

    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            // Первый ход гарантированно чист
            if (x == firstX && y == firstY) continue;
            availableCoords.emplace_back(x, y);
        }
    }

    // Перемешиваем координаты с помощью генератора случайных чисел
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(availableCoords.begin(), availableCoords.end(), g);

    // Расставляем мины
    int minesToPlace = std::min(total_mines_, static_cast<int>(availableCoords.size()));
    for (int i = 0; i < minesToPlace; ++i) {
        auto [mx, my] = availableCoords[static_cast<size_t>(i)];
        GetCell(mx, my).SetMine(true);
    }

    // Считаем соседей
    CountNeighbors();
    is_initialized_ = true;
}

void Board::CountNeighbors() {
    static constexpr int dx[] = { -1, -1, -1,  0, 0,  1, 1, 1 };
    static constexpr int dy[] = { -1,  0,  1, -1, 1, -1, 0, 1 };

    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            if (GetCell(x, y).HasMine()) continue;

            int count = 0;
            for (int i = 0; i < 8; ++i) {
                int nx = x + dx[i];
                int ny = y + dy[i];
                if (IsValidCoordinate(nx, ny) && GetCell(nx, ny).HasMine()) {
                    ++count;
                }
            }
            GetCell(x, y).SetNeighborMines(count);
        }
    }
}

bool Board::Reveal(int x, int y) {
    if (!IsValidCoordinate(x, y)) return true;

    // Если поле еще не было инициализировано, первый ход генерирует мины
    if (!is_initialized_) {
        Initialize(x, y);
    }

    Cell& cell = GetCell(x, y);

    // Если клетка помечена флагом или уже открыта - ничего не делаем
    if (cell.IsFlagged() || cell.IsRevealed()) {
        return true;
    }

    cell.Reveal();

    // Если попали на мину - поражение
    if (cell.HasMine()) {
        return false;
    }

    // Если вокруг нет мин - автоматически открываем соседей (Flood Fill)
    if (cell.NeighborMines() == 0) {
        FloodFillEmptyCells(x, y);
    }

    return true;
}

void Board::FloodFillEmptyCells(int startX, int startY) {
    static constexpr int dx[] = { -1, -1, -1,  0, 0,  1, 1, 1 };
    static constexpr int dy[] = { -1,  0,  1, -1, 1, -1, 0, 1 };

    std::queue<std::pair<int, int>> q;
    q.push({ startX, startY });

    while (!q.empty()) {
        auto [cx, cy] = q.front();
        q.pop();

        for (int i = 0; i < 8; ++i) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (IsValidCoordinate(nx, ny)) {
                Cell& neighbor = GetCell(nx, ny);
                if (!neighbor.IsRevealed() && !neighbor.IsFlagged() && !neighbor.HasMine()) {
                    neighbor.Reveal();
                    // Если у соседа тоже 0 мин вокруг, продолжаем распространение
                    if (neighbor.NeighborMines() == 0) {
                        q.push({ nx, ny });
                    }
                }
            }
        }
    }
}

void Board::ToggleFlag(int x, int y) {
    if (!IsValidCoordinate(x, y)) return;
    Cell& cell = GetCell(x, y);
    if (!cell.IsRevealed()) {
        cell.ToggleFlag();
    }
}

bool Board::CheckWinCondition() const noexcept {
    for (const auto& cell : cells_) {
        // Если хотя бы одна клетка БЕЗ мины еще закрыта - игра продолжается
        if (!cell.HasMine() && !cell.IsRevealed()) {
            return false;
        }
    }
    return true;
}

void Board::Print(bool revealMines) const {
    // Печать номеров колонок (X)
    std::cout << "\n    ";
    for (int x = 0; x < width_; ++x) {
        std::cout << x << " ";
    }
    std::cout << "\n   ";
    for (int x = 0; x < width_; ++x) {
        std::cout << "--";
    }
    std::cout << "\n";

    // Печать строк с номерами строк (Y)
    for (int y = 0; y < height_; ++y) {
        if (y < 10) std::cout << " ";
        std::cout << y << "| ";
        for (int x = 0; x < width_; ++x) {
            std::cout << GetCell(x, y).GetDisplayChar(revealMines) << " ";
        }
        std::cout << "|\n";
    }

    std::cout << "   ";
    for (int x = 0; x < width_; ++x) {
        std::cout << "--";
    }
    std::cout << "\n";
}

} // namespace minesweeper
