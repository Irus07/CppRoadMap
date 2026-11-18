#include "Game.hpp"
#include <iostream>
#include <string>

namespace minesweeper {

Game::Game(int width, int height, int total_mines)
    : board_(width, height, total_mines) {
}

void Game::Run() {
    std::cout << "========================================\n";
    std::cout << "        САПЕР (MINESWEEPER) НА C++      \n";
    std::cout << "========================================\n";
    std::cout << "Команды:\n";
    std::cout << "  o <x> <y>  - Открыть клетку (Open)\n";
    std::cout << "  f <x> <y>  - Поставить / Снять флаг (Flag)\n";
    std::cout << "  q          - Выйти из игры\n";

    while (status_ == GameStatus::InProgress) {
        Render();
        ProcessInput();

        if (status_ == GameStatus::InProgress && board_.CheckWinCondition()) {
            status_ = GameStatus::Won;
        }
    }

    Render();

    if (status_ == GameStatus::Won) {
        std::cout << "\n🎉 ПОЗДРАВЛЯЕМ! ВЫ ВЫИГРАЛИ! Все безопасные клетки открыты!\n";
    } else if (status_ == GameStatus::Lost) {
        std::cout << "\n💥 БУМ! Вы наступили на мину. ИГРА ОКОНЧЕНА!\n";
    }
}

void Game::Render() const {
    bool revealMines = (status_ != GameStatus::InProgress);
    board_.Print(revealMines);
}

void Game::ProcessInput() {
    std::cout << "\nВаш ход (например, 'o 3 4' или 'f 1 2'): ";
    char action = '\0';
    if (!(std::cin >> action)) {
        status_ = GameStatus::Lost;
        return;
    }

    if (action == 'q' || action == 'Q') {
        std::cout << "Выход из игры...\n";
        exit(0);
    }

    int x = -1;
    int y = -1;
    if (!(std::cin >> x >> y)) {
        std::cout << "Ошибка ввода! Введите действие и два числа.\n";
        std::cin.clear();
        std::string dummy;
        std::getline(std::cin, dummy);
        return;
    }

    if (!board_.IsValidCoordinate(x, y)) {
        std::cout << "Координаты вне поля! Допустимо: X от 0 до " 
                  << (board_.GetWidth() - 1) << ", Y от 0 до " 
                  << (board_.GetHeight() - 1) << ".\n";
        return;
    }

    if (action == 'o' || action == 'O') {
        bool safe = board_.Reveal(x, y);
        if (!safe) {
            status_ = GameStatus::Lost;
        }
    } else if (action == 'f' || action == 'F') {
        board_.ToggleFlag(x, y);
    } else {
        std::cout << "Неизвестная команда '" << action << "'! Используйте 'o' или 'f'.\n";
    }
}

} // namespace minesweeper
