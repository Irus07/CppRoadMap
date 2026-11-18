#include "Game.hpp"
#include <iostream>

int main() {
    // Стандартное поле 9x9 с 10 минами (Beginner level в классическом Windows Сапере)
    minesweeper::Game game(9, 9, 10);
    game.Run();
    return 0;
}
