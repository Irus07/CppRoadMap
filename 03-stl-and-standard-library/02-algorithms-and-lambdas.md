# 02. Алгоритмы STL и Лямбда-выражения

> Архитектурный шедевр STL — разделение контейнеров и алгоритмов. Алгоритмы ничего не знают о контейнерах, они работают через абстракцию **итераторов** (`begin()`, `end()`).

---

## 1. Анатомия лямбда-выражений в C++

В C# лямбда `x => x * 2` или `(x, y) => { return x + y; }` захватывает переменные в невидимый сгенерированный компилятором класс в куче (вызывает GC-аллокацию).

В C++ лямбда — это локальный функциональный объект (**функтор**) на стеке, создаваемый **без каких-либо аллокаций в куче**.

```
[ capture_clause ] ( parameters ) mutable constexpr -> return_type { body }
  ^^^^^^^^^^^^^^     ^^^^^^^^^^   ^^^^^^^ ^^^^^^^^^   ^^^^^^^^^^^^^   ^^^^^^
    Захват           Параметры     Флаги    Compile    Возвращаемый    Тело
  переменных                                -time          тип
```

### Список захвата (`capture clause`):
* `[]` — ничего не захватывать (может приводиться к обычному C-указателю на функцию!).
* `[&]` — захватить все внешние переменные **по ссылке**. (Опасно, если лямбда переживёт текущий scope!).
* `[=]` — захватить все внешние переменные **по значению (копированием)**.
* `[x, &y]` — переменную `x` скопировать, переменную `y` взять по ссылке.
* `[ptr = std::move(p)]` — Move-захват (C++14). Забирает владение `std::unique_ptr` внутрь лямбды.

### Ключевое слово `mutable`:
По умолчанию переменные, захваченные по значению `[=]`, константны внутри тела лямбды. Если их нужно изменять локально, добавьте `mutable`:

```cpp
int counter = 0;
auto inc = [counter]() mutable {
    return ++counter; // меняет копию counter внутри лямбды
};
```

---

## 2. Ключевые алгоритмы из `<algorithm>`

Никогда не пишите ручные циклы `for` там, где есть алгоритм STL. Алгоритмы протестированы, оптимизированы и могут векторизоваться (SIMD) компилятором.

### Поиск и проверка условий:
```cpp
#include <algorithm>
#include <vector>
#include <iostream>

std::vector<int> numbers = { 5, 12, 8, 30, 2, 19 };

// 1. std::find_if:
auto it = std::find_if(numbers.begin(), numbers.end(), [](int n) {
    return n > 20;
});
if (it != numbers.end()) {
    std::cout << "First > 20 is: " << *it << "\n";
}

// 2. std::all_of / any_of / none_of (аналоги All/Any в LINQ):
bool allPositive = std::all_of(numbers.begin(), numbers.end(), [](int n) { return n > 0; });
```

### Сортировка и трансформация:
```cpp
// 3. std::sort с кастомным компаратором:
std::sort(numbers.begin(), numbers.end(), [](int a, int b) {
    return a > b; // по убыванию
});

// 4. std::transform (аналог .Select() в LINQ):
std::vector<int> doubled;
doubled.reserve(numbers.size());

std::transform(numbers.begin(), numbers.end(), std::back_inserter(doubled), [](int n) {
    return n * 2;
});
```

### Агрегация (`<numeric>`):
```cpp
#include <numeric>

// 5. std::accumulate (аналог .Aggregate() / .Sum()):
int sum = std::accumulate(numbers.begin(), numbers.end(), 0);
```

---

## 3. Полиморфные лямбды (`auto`)

Начиная с C++14, параметры лямбд могут быть `auto`. Это автоматически превращает лямбду в **шаблонный функтор**:

```cpp
auto print = [](const auto& value) {
    std::cout << value << "\n";
};

print(42);            // скомпилирует инстанс для int
print("Hello World"); // скомпилирует инстанс для const char*
```

---

## 4. `std::function`: Когда нужна динамическая обёртка

Если вы хотите сохранить лямбду в поле класса или передать как callback в стиле C# `Action<T>` / `Func<T, R>`:

```cpp
#include <functional>

class Button {
public:
    std::function<void()> onClick; // Может хранить любую функцию или лямбду
};

int main() {
    Button btn;
    btn.onClick = []() { std::cout << "Clicked!\n"; };
    btn.onClick();
}
```

> **Внимание к оверхеду:** `std::function` использует type erasure и может выделять память в куче, если замыкание лямбды слишком велико. Если важна максимальная скорость, в шаблонах лучше принимать сырой тип вызываемого объекта (`template <typename F> void Run(F func)`).
