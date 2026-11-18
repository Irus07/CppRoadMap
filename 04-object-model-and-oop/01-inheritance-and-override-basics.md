# 01. Наследование и Переопределение методов (Простыми словами)

> В C# наследование интуитивно понятно: `: BaseClass`, `virtual`, `override`, `base.Method()`. В C++ логика точно такая же, но синтаксис имеет пару нюансов, о которых обязательно нужно знать.

---

## 1. Синтаксис наследования: Почему важно писать `: public`

В C# вы просто пишете имя базового класса через двоеточие:
```csharp
// C#
class Dog : Animal { }
```

В C++ после двоеточия **обязательно нужно указать спецификатор доступа** (обычно `public`):
```cpp
// C++
class Dog : public Animal { };
```

### ⚠️ Ловушка: Что будет, если забыть слово `public`?
Если написать `class Dog : Animal { };` без слова `public`:
* Компилятор посчитает наследование **`private` по умолчанию**!
* Все публичные методы класса `Animal` станут закрытыми внутри `Dog`.
* Вы не сможете вызвать методы базового класса снаружи: `dog.Eat();` выдаст ошибку доступа.

> **Правило:** В 99% случаев в C++ пишется `class Derived : public Base`.

---

## 2. Вызов конструктора базового класса

В C# вы используете ключевое слово `base(...)`:
```csharp
// В C#
public Dog(string name, int age) : base(name)
{
    this.age = age;
}
```

В C++ вместо слова `base` вы просто вызываете **имя базового класса в списке инициализации**:
```cpp
// В C++
class Animal {
protected:
    std::string name_;
public:
    Animal(std::string name) : name_(std::move(name)) {}
};

class Dog : public Animal {
private:
    int age_;
public:
    // Вызываем конструктор Animal прямо по имени:
    Dog(std::string name, int age) 
        : Animal(std::move(name)), age_(age) {
    }
};
```

---

## 3. Переопределение методов: `virtual` и `override`

Как и в C#, в C++ методы по умолчанию **не виртуальные**. Чтобы метод можно было переопределить в производном классе:
1. В базовом классе метод помечается словом **`virtual`**.
2. В производном классе метод помечается словом **`override`** в конце сигнатуры.

### Пример:
```cpp
#include <iostream>
#include <string>

class Animal {
public:
    // Виртуальный деструктор (ОБЯЗАТЕЛЕН в базовом классе!):
    virtual ~Animal() = default;

    // Виртуальный метод:
    virtual void MakeSound() const {
        std::cout << "*Some generic animal sound*\n";
    }
};

class Dog : public Animal {
public:
    // Переопределяем метод:
    void MakeSound() const override {
        std::cout << "Woof! Woof!\n";
    }
};

class Cat : public Animal {
public:
    void MakeSound() const override {
        std::cout << "Meow!\n";
    }
};
```

### Как это работает на практике:
Полиморфизм в C++ работает **только по ссылке (`&`) или указателю (`*`)**:

```cpp
void PlayWithAnimal(const Animal& pet) {
    pet.MakeSound(); // Вызовет правильный звук в зависимости от типа!
}

int main() {
    Dog dog("Rex", 3);
    Cat cat("Misty", 2);

    PlayWithAnimal(dog); // Напечатает: Woof! Woof!
    PlayWithAnimal(cat); // Напечатает: Meow!
}
```

---

## 4. Вызов метода базового класса (Аналог `base.Method()`)

В C# для вызова реализации родителя используется `base.Speak()`.

В C++ для этого используется оператор разрешения области видимости `::` с именем базового класса:

```cpp
class Warrior : public Player {
public:
    void TakeDamage(int damage) override {
        std::cout << "Warrior armor blocked 5 damage!\n";
        int reducedDamage = damage - 5;

        // Вызов метода базового класса Player:
        Player::TakeDamage(reducedDamage); 
    }
};
```

---

## 5. Абстрактные классы: Синтаксис `= 0`

В C# есть ключевое слово `abstract class` и `abstract void Method();`.

В C++ абстрактный метод называется **чисто виртуальной функцией (Pure Virtual Function)**. 
Вместо слова `abstract` в конце метода просто пишется **`= 0;`**:

```cpp
// Класс автоматически становится абстрактным, 
// если в нем есть хотя бы один метод с "= 0":
class Monster {
public:
    virtual ~Monster() = default;

    // Чисто виртуальный метод (абстрактный):
    virtual void Attack() = 0; 
};

class Zombie : public Monster {
public:
    // Обязаны реализовать метод, иначе Zombie тоже будет абстрактным:
    void Attack() override {
        std::cout << "Zombie bites for 15 damage!\n";
    }
};

int main() {
    // Monster m; // ОШИБКА: нельзя создать экземпляр абстрактного класса!
    Zombie z;     // ОК
    z.Attack();
}
```

---

## 6. Шпаргалка по терминологии (C# ➔ C++)

| Концепция ООП | В C# | В C++ |
| :--- | :--- | :--- |
| Базовый класс | `class B : A` | `class B : public A` |
| Вызов родительского конструктора | `: base(args)` | `: BaseClass(args)` |
| Вызов родительского метода | `base.Method()` | `BaseClass::Method()` |
| Разрешить переопределение | `virtual void M()` | `virtual void M()` |
| Переопределить в потомке | `override void M()` | `void M() override` |
| Абстрактный метод | `abstract void M();` | `virtual void M() = 0;` |
| Запретить дальнейшее наследование | `sealed class B` | `class B final` |
| Запретить переопределение метода | `sealed override void M()` | `void M() final override` |
