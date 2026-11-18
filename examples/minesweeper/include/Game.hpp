#pragma once

#include "Board.hpp"

namespace minesweeper {

enum class GameStatus {
    InProgress,
    Won,
    Lost
};

class Game {
public:
    Game(int width = 9, int height = 9, int total_mines = 10);

    // Запуск консольного игрового цикла
    void Run();

    GameStatus GetStatus() const noexcept { return status_; }

private:
    Board board_;
    GameStatus status_ = GameStatus::InProgress;

    void ProcessInput();
    void Render() const;
};

} // namespace minesweeper
