# 01. Умные указатели (Smart Pointers) и концепция владения

> **Золотое правило Modern C++:** Сырые указатели (`T*`) никогда не должны владеть памятью. Они служат только для наблюдения («view / observer»). Владение памятью в куче осуществляется исключительно через **умные указатели** из заголовка `<memory>`.

---

## 1. Спектр владения: три главных инструмента

```
Exclusive Ownership       Shared Ownership          Non-owning Observer
  std::unique_ptr   -->   std::shared_ptr    -->     std::weak_ptr
  (90% случаев)           (когда нужно совместное)  (разрыв циклов)
```

---

## 2. `std::unique_ptr`: Монопольное владение

`std::unique_ptr<T>` гарантирует, что объектом владеет **ровно один хозяин**. 

* **Размер в памяти:** Ровно 8 байт (размер обычного указателя, нулевой оверхед по памяти и скорости).
* **Копирование:** **Запрещено**. Его нельзя скопировать, только передать (переместить) через `std::move`.
* **Очистка:** Когда `unique_ptr` уничтожается, он автоматически вызывает `delete`.

### Пример использования:
```cpp
#include <iostream>
#include <memory>

struct Connection {
    Connection() { std::cout << "Connected\n"; }
    ~Connection() { std::cout << "Disconnected\n"; }
    void Send(const std::string& msg) { std::cout << "Sending: " << msg << "\n"; }
};

void UseConnection(std::unique_ptr<Connection> conn) {
    conn->Send("Hello");
} // Здесь conn уничтожается, вызывается деструктор Connection!

int main() {
    // Создаем через std::make_unique (C++14+)
    auto conn = std::make_unique<Connection>();

    // auto copy = conn; // ОШИБКА КОМПИЛЯЦИИ: копирование запрещено!
    UseConnection(std::move(conn)); // Передаем владение функции

    // После std::move переменная conn становится nullptr!
    if (!conn) {
        std::cout << "conn is now empty\n";
    }
}
```

---

## 3. `std::shared_ptr`: Совместное владение

Аналог ссылок в C#, но со **счётчиком ссылок (Reference Counting)**:
* При копировании `shared_ptr` счётчик увеличивается на 1.
* При выходе из зоны видимости счётчик уменьшается на 1.
* Когда счётчик достигает 0, объект в куче удаляется.

### Оверхед `shared_ptr`:
* Хранит **два** указателя (16 байт): указатель на объект + указатель на контрольный блок (Control Block).
* Счётчик ссылок атомарный (`atomic`), поэтому инкремент и декремент потокобезопасны, но стоят несколько тактов CPU.

### Важность `std::make_shared`:
```cpp
// ПЛОХО (две аллокации в куче):
std::shared_ptr<Widget> w(new Widget()); // 1 аллокация под Widget, 1 аллокация под Control Block

// ХОРОШО (одна аллокация в куче):
auto w = std::make_shared<Widget>();     // Единый блок памяти под объект и счётчик
```

---

## 4. `std::weak_ptr`: Решение проблемы циклических ссылок

В C# сборщик мусора умеет находить циклические ссылки (A ссылается на B, а B на A) и собирать их, если до них нет пути от корней GC (GC Roots).

В C++ счётчик ссылок `std::shared_ptr` при цикле **никогда не станет равен 0**. Это приведёт к пожизненной утечке памяти!

### Решение:
`std::weak_ptr` наблюдает за объектом, но **не увеличивает** счётчик владения.

```cpp
#include <iostream>
#include <memory>

struct Node {
    std::string name;
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> prev; // weak_ptr предотвращает цикл!

    Node(std::string n) : name(std::move(n)) {}
    ~Node() { std::cout << "Node " << name << " destroyed\n"; }
};

int main() {
    auto a = std::make_shared<Node>("A");
    auto b = std::make_shared<Node>("B");

    a->next = b;
    b->prev = a; // Цикл разорван благодаря weak_ptr!

    // Как получить доступ к weak_ptr?
    // Нужно временно захватить владение через .lock():
    if (auto lockedPrev = b->prev.lock()) {
        std::cout << "B's previous node is: " << lockedPrev->name << "\n";
    } else {
        std::cout << "Node A was already deleted\n";
    }
} // И a, и b успешно удалятся здесь
```

---

## 5. Взаимодействие со старыми C-библиотеками

Часто вам нужно передать сырой указатель в C-библиотеку (например, WinAPI, DirectX, curl, sqlite):

* `.get()` — получить сырой указатель без передачи владения:
  ```cpp
  auto file = std::make_unique<FileHandle>();
  legacy_c_function(file.get()); // file по-прежнему владеет объектом
  ```
* `.release()` — отказаться от владения (вернуть сырой указатель, умный указатель становится `nullptr`):
  ```cpp
  FileHandle* raw = file.release(); // Теперь вы обязаны вручную удалить raw!
  ```
* **Пользовательский Deleter (Custom Deleter):**
  Умные указатели могут вызывать не только `delete`, но и специальные функции освобождения (например, `fclose`, `free`, WinAPI `CloseHandle`):
  ```cpp
  std::unique_ptr<FILE, decltype(&fclose)> f(fopen("log.txt", "r"), &fclose);
  // При выходе из блока автоматически вызовется fclose!
  ```
