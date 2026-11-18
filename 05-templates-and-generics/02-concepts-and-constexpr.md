# 02. Концепты (Concepts, C++20) и Compile-time вычисления

> До C++20 шаблоны были болью: если вы передавали неподходящий тип в шаблон, компилятор выплёвывал простыню на 500 строк непонятных ошибок шаблонизации (SFINAE). **Концепты в C++20** навсегда решили эту проблему, дав C++ ещё более мощные ограничения, чем `where` в C#.

---

## 1. Сравнение с ограничениями в C#

### В C#:
```csharp
public T Add<T>(T a, T b) where T : INumber<T> 
{
    return a + b;
}
```

### В C++20:
```cpp
#include <concepts>

// Вариант 1: прямо вместо typename
template <std::integral T>
T Add(T a, T b) {
    return a + b;
}

// Вариант 2: через ключевое слово requires (очень похоже на where в C#):
template <typename T>
    requires std::integral<T>
T Add(T a, T b) {
    return a + b;
}

// Вариант 3: сокращенный синтаксис (Terse Syntax):
auto Add(std::integral auto a, std::integral auto b) {
    return a + b;
}
```

Если вы передадите `Add("Hello", "World")`, компилятор C++ выдаст ровно **одну аккуратную строчку**:
`error: constraints not satisfied for class std::basic_string<char>`.

---

## 2. Создание собственных концептов

В C# ограничения привязаны к иерархии типов (наследование от интерфейса или базового класса). 
В C++20 концепт может проверять **любые синтаксические свойства типа**:

```cpp
#include <concepts>
#include <string>
#include <iostream>

// Создаем концепт: "Тип обязан иметь метод Serialize(), возвращающий std::string"
template <typename T>
concept Serializable = requires(T obj) {
    { obj.Serialize() } -> std::same_as<std::string>;
};

// Функция, принимающая только сериализуемые типы:
void SaveToDisk(const Serializable auto& item) {
    std::string data = item.Serialize();
    std::cout << "Saved: " << data << "\n";
}

struct User {
    std::string Serialize() { return "{ user: 'Ivan' }"; }
};

struct Dog {
    void Bark(); // Нет метода Serialize
};

int main() {
    User u;
    SaveToDisk(u); // ОК!

    Dog d;
    // SaveToDisk(d); // ОШИБКА КОМПИЛЯЦИИ: Dog не удовлетворяет концепту Serializable!
}
```

---

## 3. `if constexpr`: Ветвление во время компиляции (C++17)

В C# ветка `if` всегда существует в скомпилированном IL коде и проверяется в рантайме.

В C++ `if constexpr` отсекает невыполненную ветку **ещё во время компиляции**! Код из ложной ветки вообще не попадает в итоговый исполняемый файл.

```cpp
#include <iostream>
#include <type_traits>

template <typename T>
void Inspect(T val) {
    if constexpr (std::is_pointer_v<T>) {
        // Эта ветка компилируется ТОЛЬКО если T — указатель!
        std::cout << "Pointer with value: " << *val << "\n";
    } else if constexpr (std::is_integral_v<T>) {
        std::cout << "Integer: " << val << "\n";
    } else {
        std::cout << "Other type: " << val << "\n";
    }
}

int main() {
    int x = 100;
    Inspect(x);   // Скомпилирует только вторую ветку
    Inspect(&x);  // Скомпилирует только первую ветку (*val валидно!)
}
```
Обратите внимание: для типа `int` разыменование `*val` было бы синтаксической ошибкой. Но благодаря `if constexpr` компилятор даже не пытается его компилировать, если `T` не указатель!
