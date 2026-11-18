# 04. Правила именования (Naming Conventions) и стиль кода в C++

> **Культурный шок после C#:** В мире .NET компания Microsoft утвердила единый стандарт (Framework Design Guidelines), которого придерживаются все: `PascalCase` для классов и методов, `camelCase` для локальных переменных. 
> В C++ единого официального стандарта нет, но есть **железные правила стандарта языка** о том, какие имена **строго запрещено использовать**, и общепринятые индустриальные стили.

---

## 1. КАТЕГОРИЧЕСКИ ЗАПРЕЩЕНО: Имена, зарезервированные стандартом

В C# программисты часто пишут приватные поля с подчёркиванием: `private int _health;` или `private string _name;`.

В C++ использование таких имён может привести к **Undefined Behavior** или ошибкам компиляции, потому что они зарезервированы для создателей компилятора и стандартной библиотеки!

### ❌ Что нельзя использовать в своём коде:
1. **Подчёркивание, за которым следует заглавная буква:**
   * ❌ `_Name`, `_Value`, `_Size`, `_DATA` — **ЗАПРЕЩЕНО ВЕЗДЕ**. Они зарезервированы для внутренних макросов компилятора.
2. **Два подчёркивания подряд (в любом месте имени):**
   * ❌ `__my_var`, `temp__value`, `do_work__` — **ЗАПРЕЩЕНО ВЕЗДЕ**. (Например, `__cplusplus`, `__LINE__` зарезервированы).
3. **Одинарное подчёркивание в начале в глобальном пространстве имён:**
   * ❌ `int _global_counter = 0;` — зарезервировано для runtime C/C++.

> **Вывод:** Никогда не начинайте свои идентификаторы с подчёркивания и никогда не пишите двойное подчёркивание.

---

## 2. Три доминирующих стиля в индустрии

В крупных проектах на C++ обычно выбирают один из трёх стилей:

| Что именуем | 1. Google Style (Open Source) | 2. GameDev / Microsoft (Unreal/Win) | 3. STL Style (Библиотечный) |
| :--- | :--- | :--- | :--- |
| **Класс / Структура** | `PascalCase` (`PlayerManager`) | `PascalCase` (`PlayerManager`) | `snake_case` (`player_manager`) |
| **Методы** | `PascalCase` (`TakeDamage`) | `PascalCase` (`TakeDamage`) | `snake_case` (`take_damage`) |
| **Локальные переменные** | `snake_case` (`total_score`) | `camelCase` (`totalScore`) | `snake_case` (`total_score`) |
| **Приватные поля** | `snake_case_` (`health_`) | `m_health` или `mHealth` | `health_` (`_` на конце) |
| **Константы** | `kPascalCase` (`kMaxPlayers`) | `PascalCase` или `ALL_CAPS` | `kMaxPlayers` или `snake_case` |
| **Enum Class** | `PascalCase` (`Color::Red`) | `PascalCase` (`Color::Red`) | `snake_case` (`color::red`) |

---

## 3. Рекомендуемый стандарт для изучения (Modern Pragmatic Style)

Для комфортного перехода с C# мы рекомендуем наиболее чистый, современный и принятый в большинстве новых проектов стиль (**Google / Core Guidelines Hybrid**):

```cpp
#pragma once
#include <string>

// 1. Пространства имен (Namespaces): только строчные буквы без подчеркиваний
namespace gamelogic {

// 2. Константы времени компиляции: префикс 'k' + PascalCase
constexpr int kDefaultHealth = 100;

// 3. Перечисления: ВСЕГДА используйте 'enum class' (Scoped Enums) + PascalCase
enum class PlayerState {
    Idle,
    Walking,
    Attacking
};

// 4. Классы и структуры: PascalCase
class GameCharacter {
private:
    // 5. Приватные поля: snake_case с подчёркиванием НА КОНЦЕ (trailing underscore)
    std::string name_;
    int health_ = kDefaultHealth;
    bool is_alive_ = true;

public:
    // 6. Конструктор: аргументы без подчеркивания идеально ложатся в список инициализации!
    GameCharacter(std::string name, int health)
        : name_(std::move(name)), health_(health) {
    }

    // 7. Публичные методы: PascalCase (привычно для C#)
    void TakeDamage(int damage_amount) {
        // 8. Локальные переменные и параметры: snake_case или camelCase
        health_ -= damage_amount;
        if (health_ <= 0) {
            health_ = 0;
            is_alive_ = false;
        }
    }

    // 9. Геттеры: const на конце обязателен
    int GetHealth() const noexcept { 
        return health_; 
    }
    
    bool IsAlive() const noexcept { 
        return is_alive_; 
    }
};

} // namespace gamelogic
```

---

## 4. Почему подчёркивание на конце (`name_`) лучше префикса `m_name`?

В старом коде Visual Studio вы часто увидите венгерскую нотацию или префиксы `m_` (*member*): `m_strName`, `m_health`.

В современном C++ большинство разработчиков перешли на **подчёркивание в конце (`name_`)**:
1. **Чистое автодополнение (IntelliSense):** Когда вы нажимаете точку или стрелочку в IDE, если все поля начинаются на `m_`, вам приходится пролистывать десятки полей на букву «M». С суффиксом `name_` вы сразу печатаете первую букву имени.
2. **Красивые конструкторы:**
   ```cpp
   // Имя аргумента name совпадает по смыслу с полем name_:
   Player(std::string name) : name_(std::move(name)) {}
   ```
   Вам не нужно придумывать костыльные имена аргументов вроде `_name`, `aName`, `theName` или писать `this->name = name`.

---

## 5. Именование файлов (`.hpp` и `.cpp`)

В отличие от C# и Windows, где файловая система регистронезависима (`player.cs` и `Player.cs` — один и тот же файл), **в Linux и сборках на серверах регистр символов имеет значение**!

### Два популярных подхода:
1. **`PascalCase` (1-в-1 совпадает с именем класса):**
   * `PlayerController.hpp`
   * `PlayerController.cpp`
   * *Плюс:* Сразу видно, какой класс лежит внутри (привычно для разработчиков C# и Java).
2. **`snake_case` или `kebab-case` (предпочтителен для кроссплатформенных проектов):**
   * `player_controller.hpp` / `player-controller.hpp`
   * `player_controller.cpp` / `player-controller.cpp`
   * *Плюс:* Гарантия 100% совместимости с Linux/macOS и серверами CI/CD (исключены случайные конфликты регистра вроде `Player.hpp` и `player.hpp`).

> **Расширения файлов:**
> Для заголовочных файлов используйте **`.hpp`** (или `.h`), а для исходного кода — **`.cpp`**. Расширение `.hpp` явно сигнализирует другим разработчикам, что это заголовок C++, а не чистый Си.

---

## 6. Именование интерфейсов

В C# для интерфейсов строго обязателен префикс `I`: `IEnumerable`, `IDisposable`.

В C++ префикс `I` **не является требованием языка**, но в прикладном коде и геймдеве его активно используют по договоренности:
```cpp
class ISerializable {
public:
    virtual ~ISerializable() = default;
    virtual void Serialize(std::ostream& out) const = 0;
};
```
Это полезная и чистая практика, перекочевавшая из C#, которую стоит сохранить.
