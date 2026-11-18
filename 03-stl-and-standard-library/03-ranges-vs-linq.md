# 03. C++20 Ranges vs C# LINQ: Битва пайплайнов

> Для разработчика на C# библиотека **C++20 Ranges** покажется родным домом: пайплайны обработки данных, фильтрация, проекции и ленивые вычисления (`views`). Но под капотом — принципиальная разница в стоимости абстракций.

---

## 1. Сравнение философии

| Характеристика | C# LINQ (`IEnumerable<T>`) | C++20 Ranges (`std::views`) |
| :--- | :--- | :--- |
| **Интерфейс** | Методы расширения (`.Where().Select()`) | Пайплайн оператор (`\| std::views::filter`) |
| **Вычисления** | Ленивые (yield return, итератор) | **Ленивые (lazy)** |
| **Аллокации в куче** | Каждое звено создаёт объект итератора в куче (`WhereSelectEnumerableIterator`) | **0 байт аллокаций в куче!** Все адаптеры создаются на стеке как вложенные шаблонные типы |
| **Виртуальные вызовы**| Вызовы интерфейсов `IEnumerator.MoveNext()` | Инлайнятся компилятором напрямую в машинный ассемблер |

---

## 2. Параллельное сравнение кода

### Задача:
Взять массив чисел, отфильтровать только чётные, возвести в квадрат, взять первые 3 элемента и вывести на экран.

#### На C# (LINQ):
```csharp
using System;
using System.Linq;

int[] numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

var result = numbers
    .Where(n => n % 2 == 0)
    .Select(n => n * n)
    .Take(3);

foreach (var n in result)
{
    Console.WriteLine(n);
}
```

#### На C++ (C++20 Ranges):
```cpp
#include <iostream>
#include <vector>
#include <ranges>

int main() {
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    // Красивый pipe-синтаксис:
    auto result = numbers 
        | std::views::filter([](int n) { return n % 2 == 0; })
        | std::views::transform([](int n) { return n * n; })
        | std::views::take(3);

    for (int n : result) {
        std::cout << n << "\n";
    }
}
```

---

## 3. Таблица соответствия операторов

| Что хотим сделать | В C# (LINQ) | В C++20 (std::views) |
| :--- | :--- | :--- |
| Фильтрация | `.Where(predicate)` | `std::views::filter(pred)` |
| Проекция / Преобразование | `.Select(selector)` | `std::views::transform(func)` |
| Ограничение количества | `.Take(count)` | `std::views::take(count)` |
| Пропуск элементов | `.Skip(count)` | `std::views::drop(count)` |
| Разворот последовательности | `.Reverse()` | `std::views::reverse` |
| Генерация диапазона чисел | `Enumerable.Range(1, 10)` | `std::views::iota(1, 11)` |
| Распаковка пар ключей/значений | `.Select(kv => kv.Key)` | `std::views::keys` / `values` |
| Разделение строк | `str.Split(',')` | `std::views::split(',')` |

---

## 4. Материализация в коллекцию (`std::ranges::to`)

В C# вы часто завершаете цепочку вызовом `.ToList()` или `.ToArray()`.

* **В C++23:** появился прямой аналог:
  ```cpp
  auto vec = result | std::ranges::to<std::vector>();
  ```
* **В C++20:** результат оборачивается в конструктор вектора:
  ```cpp
  auto intermediate = numbers | std::views::filter(...);
  std::vector<int> vec(intermediate.begin(), intermediate.end());
  ```

---

## 5. Главный плюс Ranges для C++ программиста
В отличие от старого подхода STL с итераторами `std::sort(v.begin(), v.end())`, с ranges можно передавать контейнер целиком:

```cpp
std::ranges::sort(numbers); // Больше не нужно писать begin() и end()!
```
Компилятор сам проверяет границы, а шаблонный код разворачивается в цикл без накладных расходов.
