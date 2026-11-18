# 02. Множественное наследование и имитация интерфейсов

> В C# запрещено множественное наследование классов (разрешена только реализация нескольких `interface`). В C++ разрешено множественное наследование **любых классов**. Однако современный стиль C++ рекомендует использовать его осторожно.

---

## 1. Как создавать интерфейсы в C++

В C++ нет ключевого слова `interface`. Интерфейс — это обычный класс, у которого:
1. Все методы чисто виртуальные (`= 0`).
2. Виртуальный деструктор по умолчанию (`virtual ~IInterface() = default;`).
3. Нет полей данных.

```cpp
class ISerializable {
public:
    virtual ~ISerializable() = default;
    virtual void Serialize(std::ostream& out) const = 0;
};

class IPrintable {
public:
    virtual ~IPrintable() = default;
    virtual void Print() const = 0;
};

// Реализация нескольких интерфейсов:
class Document : public ISerializable, public IPrintable {
public:
    void Serialize(std::ostream& out) const override { /* ... */ }
    void Print() const override { /* ... */ }
};
```

---

## 2. Ромбовидное наследование (The Diamond Problem)

Проблема возникает, когда два класса наследуются от общего предка, а третий класс наследуется от них обоих:

```
      Device
     /      \
  Printer   Scanner
     \      /
    Copier
```

### Код с ошибкой:
```cpp
class Device {
public:
    int id = 0;
};

class Printer : public Device {};
class Scanner : public Device {};

class Copier : public Printer, public Scanner {};

int main() {
    Copier c;
    // c.id = 10; // ОШИБКА КОМПИЛЯЦИИ: Неоднозначность! 
                  // В объекте Copier сейчас ДВА экземпляра Device::id!
    
    // Приходится уточнять:
    c.Printer::id = 10;
    c.Scanner::id = 20;
}
```

---

## 3. Решение: Виртуальное наследование (`virtual public`)

Чтобы предок существовал в единственном экземпляре, используют ключевое слово `virtual`:

```cpp
class Device {
public:
    int id = 0;
};

// Виртуальное наследование:
class Printer : virtual public Device {};
class Scanner : virtual public Device {};

// Теперь в Copier будет ровно один экземпляр Device:
class Copier : public Printer, public Scanner {};

int main() {
    Copier c;
    c.id = 42; // ОК: неоднозначности больше нет!
}
```

> **Цена виртуального наследования:** Дополнительные смещения и указатели в памяти объекта. Поэтому наследование с данными от нескольких классов используется редко; предпочитают **композицию** («has-a» вместо «is-a»).

---

## 4. Приведение типов: `dynamic_cast` vs `static_cast`

В C# вы используете операторы `as` и `is`: `var p = obj as Printer;`.

В C++ есть специальная система приведений типов:

### 1. `dynamic_cast<T*>` (Аналог `as` в C#):
* Использует RTTI (Run-Time Type Information).
* Проверяет иерархию во время выполнения программы.
* Если приведение невозможно: для указателей возвращает `nullptr`, для ссылок бросает `std::bad_cast`.
* **Требует, чтобы базовый класс имел хотя бы одну виртуальную функцию.**

```cpp
Device* dev = GetUnknownDevice();

if (Printer* p = dynamic_cast<Printer*>(dev)) {
    // Успешно! dev действительно является Printer
    p->PrintJob();
}
```

### 2. `static_cast<T>` (Быстрое приведение во время компиляции):
* Не производит проверок во время выполнения (0 накладных расходов).
* Если вы ошиблись с типом — вы получаете **Undefined Behavior** (UB) и порчу памяти.
* Применяется для:
  * Преобразования примитивов (`double` в `int`).
  * Downcasting, когда программист на 100% уверен в типе объекта.
