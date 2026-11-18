# 02. Корутины C++20 vs Async/Await в C#

> Вы привыкли к удобному `async Task<int> FetchDataAsync()` в C#. В C++20 тоже появились ключевые слова `co_await`, `co_yield` и `co_return`. Но их философия кардинально отличается.

---

## 1. Концептуальная разница

| Аспект | C# `async / await` | C++20 Coroutines |
| :--- | :--- | :--- |
| **Уровень абстракции** | Высокоуровневая система задач (`Task`, `ValueTask`) | **Низкоуровневый строительный блок (Core Language Feature)** |
| **Планировщик (Scheduler)**| Встроенный .NET ThreadPool и SynchronizationContext | **Отсутствует в стандарте!** Вы сами решаете, где и как возобновлять корутину |
| **Аллокации памяти** | Объект задачи часто аллоцируется в куче (если не `ValueTask`) | Компилятор пытается оптимизировать и устранить аллокацию фрейма корутины (**HALO** — Heap Allocation elision to Local Object) |
| **Тип корутин** | Stackless (без отдельного стека) | Stackless (компилятор преобразует функцию в автомат состояний) |

---

## 2. Три ключевых слова C++20

Любая функция, содержащая хотя бы одно из этих трёх ключевых слов, автоматически становится корутиной:

1. **`co_await <expr>`:** приостановить выполнение текущей функции до завершения операции (аналог `await`).
2. **`co_yield <expr>`:** вернуть промежуточное значение вызывающему коду и приостановиться (аналог `yield return` в C#).
3. **`co_return <expr>`:** завершить работу корутины и вернуть итоговый результат.

---

## 3. Использование генераторов (Аналог `IEnumerable` + `yield return`)

В C# вы часто пишете:
```csharp
IEnumerable<int> GetNumbers()
{
    yield return 1;
    yield return 2;
}
```

В C++23 для этого появился стандартный тип **`std::generator<T>`**:

```cpp
#include <iostream>
#include <generator> // C++23

std::generator<int> GetFibonacci(int count) {
    int a = 0, b = 1;
    for (int i = 0; i < count; ++i) {
        co_yield a;
        auto next = a + b;
        a = b;
        b = next;
    }
}

int main() {
    for (int val : GetFibonacci(10)) {
        std::cout << val << " ";
    }
    std::cout << "\n";
}
```

---

## 4. Как писать асинхронный I/O код в C++?

Поскольку в стандартной библиотеке C++ нет встроенного планировщика задач (как .NET TaskScheduler), для сетевого и асинхронного программирования сообщество использует готовые проверенные библиотеки:

1. **`boost::asio` / `asio` (Standalone):** Промышленный стандарт асинхронного сетевого ввода-вывода (IO completion ports на Windows, epoll на Linux). Имеет нативную поддержку `co_await`.
2. **`libunifex` / P2300 (std::execution):** Будущий стандарт асинхронности C++26 на базе паттерна Senders/Receivers.
