# 02. Перегрузка операторов: Как подружить свой класс с cout, == и +

> В C# для вывода объекта в консоль вы переопределяете метод `.ToString()`, а для сравнения — `Equals()`. В C++ для этого используют **перегрузку операторов**. Она позволяет объектам вашего класса вести себя так же естественно, как стандартным числам `int` или `double`.

---

## 1. Как научить свой класс выводиться через `std::cout` (Аналог `ToString`)

В C# вы пишете `Console.WriteLine(player)` и переопределяете `public override string ToString()`.

В C++ для вывода в поток `std::cout << player;` перегружают оператор **`<<`**:

```cpp
#include <iostream>
#include <string>

class Player {
private:
    std::string name_;
    int score_ = 0;

public:
    Player(std::string name, int score) 
        : name_(std::move(name)), score_(score) {}

    // Ключевое слово friend позволяет внешней функции 
    // получить доступ к приватным полям name_ и score_:
    friend std::ostream& operator<<(std::ostream& os, const Player& p) {
        os << "Player(" << p.name_ << ", Score: " << p.score_ << ")";
        return os; // Возвращаем поток для поддержки цепочек: cout << a << b;
    }
};

int main() {
    Player p("Alice", 150);
    // Теперь это работает легко и красиво!
    std::cout << p << "\n"; // Напечатает: Player(Alice, Score: 150)
}
```

---

## 2. Сравнение объектов: `operator==`

В C# вы переопределяете `Equals()` и `GetHashCode()`, а также операторы `==` и `!=`.

### В C++20 (Современный способ — ОДНА СТРОЧКА!):
В C++20 появился оператор трехстороннего сравнения («Spaceship» `<=>`). Если вы хотите, чтобы ваш класс умел сравниваться на равенство по всем полям, достаточно написать:

```cpp
struct Point {
    int x = 0;
    int y = 0;

    // Компилятор сам сгенерирует операторы == и !=:
    bool operator==(const Point& other) const = default;
};

int main() {
    Point a{ 1, 2 };
    Point b{ 1, 2 };
    Point c{ 3, 4 };

    if (a == b) {
        std::cout << "a and b are equal!\n";
    }
    if (a != c) {
        std::cout << "a and c are not equal!\n";
    }
}
```

### Классический способ (если нужна кастомная логика сравнения):
```cpp
class Item {
private:
    int id_;
    std::string title_;

public:
    Item(int id, std::string title) : id_(id), title_(std::move(title)) {}

    bool operator==(const Item& other) const {
        // Считаем предметы равными, если совпадают их id:
        return id_ == other.id_;
    }

    bool operator!=(const Item& other) const {
        return !(*this == other); // Используем уже написанный operator==
    }
};
```

---

## 3. Арифметические операторы: Например, сложение `Vector2D`

Представьте класс двумерного вектора в игре:

```cpp
#include <iostream>

struct Vector2D {
    float x = 0.0f;
    float y = 0.0f;

    // Оператор += (меняет текущий объект)
    Vector2D& operator+=(const Vector2D& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    // Оператор + (возвращает новый объект)
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D{ x + other.x, y + other.y };
    }
};

int main() {
    Vector2D pos{ 10.0f, 20.0f };
    Vector2D velocity{ 1.0f, 2.0f };

    Vector2D newPos = pos + velocity; // Сложение объектов в одну строчку!
    std::cout << "New X: " << newPos.x << ", New Y: " << newPos.y << "\n";
}
```

---

## 4. Главное правило: Не злоупотребляйте перегрузкой

Перегружайте операторы **только тогда, когда их смысл интуитивно очевиден** для любого человека:
* `+` для векторов, комплексных чисел, матриц, денег или сложения строк — это понятно.
* `*` для умножения матрицы на скаляр — это понятно.
* ❌ `operator+` для добавления пользователя в базу данных `db + user;` — это антипаттерн! Для таких действий пишите обычные методы: `db.AddUser(user);`.
