# 03. Move-семантика и Правило пяти (Rule of Five)

> **Зачем это нужно:** До C++11 при возврате вектора из 1 000 000 элементов компилятору приходилось копировать все элементы в новую область памяти, а старую удалять. **Move-семантика** позволяет «украсть» внутренний указатель на буфер за пару процессорных тактов, сделав копирование ресурса нулевым по стоимости.

---

## 1. Lvalue и Rvalue за 1 минуту

* **Lvalue (Left Value):** Объект, у которого **есть имя и постоянный адрес в памяти**. Существует дольше одной строчки кода.
  ```cpp
  int a = 42; // 'a' — это lvalue
  std::string s = "Hello"; // 's' — это lvalue
  ```
* **Rvalue (Right Value):** Временный безымянный объект, результат выражения или литерал. Исчезает в конце строки выражения.
  ```cpp
  int b = a + 5; // результат (a + 5) — это временное rvalue
  std::string full = s + " World"; // (s + " World") — это временное rvalue
  ```

---

## 2. Что такое `std::move` на самом деле?

> **ГЛАВНЫЙ МИФ:** `std::move` ничего не перемещает и не генерирует машинного кода!

`std::move` — это просто **статическое приведение типа к rvalue-ссылке (`static_cast<T&&>`)**. 
Вы говорите компилятору: *«Я больше не претендую на данные этой переменной. Тот, кто её принимает, может безопасно забрать (украсть) её внутренние ресурсы»*.

```cpp
std::string str1 = "Large text with lots of characters...";
// Было (дорогое копирование буфера):
// std::string str2 = str1; 

// Стало (кража указателя на строку за 0 наносекунд):
std::string str2 = std::move(str1); 

// Теперь str2 владеет строкой. str1 опустошен (в безопасном валидном состоянии).
```

---

## 3. Правило нуля, трёх и пяти (Rule of 0 / 3 / 5)

### Правило нуля (Rule of Zero) — предпочтительный стиль!
Если класс содержит поля, которые сами правильно управляют своими ресурсами (`std::vector`, `std::string`, `std::unique_ptr`), **не пишите ни одного специального метода вручную**. Компилятор сгенерирует их идеально сам.

### Правило трёх (C++98)
Если ваш класс напрямую владеет сырым ресурсом (сырой указатель `T*`, дескриптор сокета), вам нужно вручную написать:
1. **Деструктор** (`~T()`)
2. **Конструктор копирования** (`T(const T& other)`)
3. **Оператор присваивания копированием** (`T& operator=(const T& other)`)

### Правило пяти (Modern C++11+)
С появлением move-семантики к Правилу трех добавляются ещё два метода:
4. **Конструктор перемещения** (`T(T&& other) noexcept`)
5. **Оператор присваивания перемещением** (`T& operator=(T&& other) noexcept`)

---

## 4. Живой пример: Класс с ручным управлением буфером

Разберём эталонный класс `Buffer`, чтобы увидеть разницу между Copy и Move:

```cpp
#include <iostream>
#include <utility> // std::exchange

class Buffer {
private:
    size_t size_ = 0;
    int* data_ = nullptr;

public:
    // 1. Обычный конструктор
    explicit Buffer(size_t size) 
        : size_(size), data_(new int[size]{}) {
        std::cout << "Allocated " << size_ << " ints\n";
    }

    // 2. Деструктор (освобождает память)
    ~Buffer() {
        delete[] data_;
        std::cout << "Destroyed\n";
    }

    // 3. Конструктор копирования (Deep Copy - дорого!)
    Buffer(const Buffer& other) 
        : size_(other.size_), data_(new int[other.size_]) {
        for (size_t i = 0; i < size_; ++i) data_[i] = other.data_[i];
        std::cout << "Deep Copied\n";
    }

    // 4. Оператор присваивания копированием
    Buffer& operator=(const Buffer& other) {
        if (this == &other) return *this; // Защита от самоприсваивания (b = b)

        delete[] data_; // Удаляем старый буфер

        size_ = other.size_;
        data_ = new int[size_];
        for (size_t i = 0; i < size_; ++i) data_[i] = other.data_[i];
        std::cout << "Copy Assigned\n";
        return *this;
    }

    // 5. Конструктор перемещения (Move Constructor - мгновенно!)
    Buffer(Buffer&& other) noexcept 
        : size_(std::exchange(other.size_, 0)),
          data_(std::exchange(other.data_, nullptr)) {
        // Мы забрали указатель other.data_, а у other обнулили его!
        std::cout << "Moved (stealing resource)\n";
    }

    // 6. Оператор присваивания перемещением
    Buffer& operator=(Buffer&& other) noexcept {
        if (this == &other) return *this;

        delete[] data_; // Освобождаем наш текущий ресурс

        // Крадем ресурсы other
        size_ = std::exchange(other.size_, 0);
        data_ = std::exchange(other.data_, nullptr);
        std::cout << "Move Assigned\n";
        return *this;
    }
};
```

---

## 5. Почему важен `noexcept` на move-конструкторе?

Обратите внимание на спецификатор `noexcept` у перемещающих операций:
```cpp
Buffer(Buffer&& other) noexcept
```

Когда `std::vector` увеличивает свой размер и переносит элементы в новый буфер памяти:
* Если move-конструктор помечен `noexcept`, вектор будет **быстро перемещать** элементы.
* Если `noexcept` забыт, `std::vector` испугается выброса исключения на полпути и откатится к **медленному глубокому копированию** всех элементов для сохранения strong exception guarantee.
