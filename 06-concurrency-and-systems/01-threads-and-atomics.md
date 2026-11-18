# 01. Многопоточность: Потоки, Блокировки и Атомики

> **Опасность C++ №1 в многопоточности:** Состояние гонки данных (Data Race) в C# приводит к некорректным данным. В C++ Data Race — это **Undefined Behavior**. Компилятор при оптимизации имеет право исходить из того, что гонок не бывает, и может выбросить проверки или зациклить чтение регистра.

---

## 1. Потоки: `std::thread` vs `std::jthread` (C++20)

### Проблема со старым `std::thread`:
Если у объекта `std::thread` вызывается деструктор, а вы забыли вызвать `.join()` или `.detach()`, программа **немедленно аварийно завершается (`std::terminate`)**!

### Решение: `std::jthread` (Joining Thread):
В C++20 появился `std::jthread`:
* Автоматически вызывает `.join()` в своём деструкторе (RAII).
* Имеет встроенный токен отмены (`std::stop_token` — прямой аналог `CancellationToken` из C#).

```cpp
#include <iostream>
#include <thread>
#include <chrono>

void Worker(std::stop_token stopToken) {
    while (!stopToken.stop_requested()) {
        std::cout << "Working...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    std::cout << "Worker stopped cleanly.\n";
}

int main() {
    {
        std::jthread t(Worker);
        std::this_thread::sleep_for(std::chrono::milliseconds(700));
        // При выходе из блока скобок деструктор jthread:
        // 1. Пошлет сигнал остановки (stopToken.request_stop())
        // 2. Дождется завершения потока (t.join())
    }
    std::cout << "Main finished.\n";
}
```

---

## 2. RAII-блокировки: `lock_guard` и `scoped_lock`

В C# вы пишете `lock(obj) { ... }`.
В C++ используются мьютексы и обёртки блокировок:

```cpp
#include <mutex>

std::mutex g_mutex;
int g_counter = 0;

void Increment() {
    // std::lock_guard захватывает мьютекс в конструкторе и
    // гарантированно отпускает в деструкторе:
    std::lock_guard<std::mutex> lock(g_mutex);
    ++g_counter;
}
```

### Защита от взаимных блокировок (Deadlock) через `std::scoped_lock` (C++17):
Если нужно заблокировать сразу два мьютекса (например, перевод денег с одного счёта на другой):

```cpp
void Transfer(Account& from, Account& to, double amount) {
    // Блокирует ОБА мьютекса одновременно по алгоритму предотвращения дэдлоков:
    std::scoped_lock lock(from.mtx, to.mtx);

    from.balance -= amount;
    to.balance += amount;
}
```

---

## 3. Атомики и Модель памяти (`std::atomic<T>`)

Для простых счётчиков и флагов мьютексы слишком дороги. Используйте `std::atomic`:

```cpp
#include <atomic>

std::atomic<int> readyCounter{ 0 };

void ThreadWork() {
    // Атомарный инкремент (на x86 транслируется в ассемблерную инструкцию 'lock inc')
    readyCounter.fetch_add(1); 
    
    // Перегруженный оператор:
    ++readyCounter; 
}
```

### Порядки памяти (Memory Ordering):
По умолчанию операции над `std::atomic` используют строжайший режим **`std::memory_order_seq_cst`** (Sequential Consistency).
Для экстремальной производительности в low-latency коде можно ослаблять барьеры памяти:
* `std::memory_order_relaxed` — атомарность без синхронизации видимости других переменных.
* `acquire` / `release` — классическая модель публикации данных между двумя потоками без полной блокировки шины.
