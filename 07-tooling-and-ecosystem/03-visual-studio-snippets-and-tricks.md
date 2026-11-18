# 03. Сниппеты и горячие клавиши Visual Studio для C++

> В C# в Visual Studio вы привыкли к `prop + Tab + Tab`, `ctor + Tab + Tab` и мгновенному переходу между файлами. В C++ для Visual Studio есть мощнейшая система сниппетов и хоткеев, которая ускоряет написание кода в разы.

---

## 1. Встроенные сниппеты Visual Studio для C++

Как использовать: вводите короткое имя (trigger) и нажимаете **`Tab + Tab`** (или один раз `Tab` в зависимости от настроек IntelliCode).

| Сниппет | Что разворачивает | Назначение |
| :--- | :--- | :--- |
| `class` | Каркас класса с конструктором и деструктором | Создание нового класса |
| `struct` | Каркас структуры | Быстрое объявление структуры |
| `ctor` | Конструктор текущего класса по умолчанию | Быстро объявить конструктор |
| `for` | Стандартный индексный цикл `for (size_t i = 0; i < n; ++i)` | Проход по индексу |
| `rfor` / `forr` | Range-based for: `for (auto&& item : collection)` | Цикл по элементам (аналог `foreach` в C#) |
| `while` | Цикл `while (condition) { }` | Базовый цикл |
| `do` | Цикл `do { } while (condition);` | Цикл с постусловием |
| `switch` | Конструкция `switch (expr) { case ...: break; default: break; }` | Ветвление |
| `try` | Блок `try { } catch (const std::exception& e) { }` | Обработка исключений |
| `namespace` | Блок `namespace name { }` | Оборачивание в пространство имён |
| `#if` | Директива препроцессора `#if ... #endif` | Условная компиляция |

---

## 2. Оборачивание кода (Surround With): `Ctrl + K, Ctrl + S`

Выделите любой фрагмент кода и нажмите сочетание клавиш **`Ctrl + K, затем Ctrl + S`** (или правая кнопка мыши ➔ *Окружить фрагментом...*):

Появится всплывающее меню:
* Обернуть в `if` / `if-else`
* Обернуть в `try-catch`
* Обернуть в `#pragma region` ... `#pragma endregion`
* Обернуть в `namespace`

---

## 3. Самый важный шорткат для C++: `Alt + O`

> **`Alt + O` — Переключение между Header (`.h`) и Source (`.cpp`) файлом.**

В C# у вас один файл `.cs`. В C++ каждый класс разделен на два файла: заголовок `.hpp` и реализацию `.cpp`. 
Клавиша `Alt + O` в Visual Studio мгновенно переключает вас между объявлением функции в `.h` и её определением в `.cpp`. Вы будете нажимать её сотни раз в день.

### Другие обязательные хоткеи Visual Studio:

| Сочетание клавиш | Действие |
| :--- | :--- |
| **`Alt + O`** | Мгновенно переключиться между `.h` / `.hpp` и `.cpp` |
| **`F12`** | Перейти к определению (Go to Definition) |
| **`Ctrl + F12`** | Перейти к объявлению (Go to Declaration) |
| **`Ctrl + ,`** (или `Ctrl + T`) | Найти всё (Go to All: файлы, классы, методы, символы) |
| **`Ctrl + .`** (или `Alt + Enter`) | Быстрые действия и рефакторинг (генерация методов в `.cpp`, инклуды) |
| **`Ctrl + K, Ctrl + C`** | Закомментировать блок кода |
| **`Ctrl + K, Ctrl + U`** | Раскомментировать блок кода |
| **`Shift + Alt + F10`** | Меню автоматического создания реализации метода в `.cpp` |

---

## 4. Как создать свой собственный сниппет

Давайте создадим сниппет, которого очень не хватает в Visual Studio по умолчанию: **заголовочный файл с `#pragma once` и пространством имён**.

### Шаг 1. Создаем файл сниппета `header.snippet`
Сниппеты в Visual Studio описываются XML-файлами с расширением `.snippet`:

```xml
<?xml version="1.0" encoding="utf-8"?>
<CodeSnippets xmlns="http://schemas.microsoft.com/VisualStudio/2005/CodeSnippet">
  <CodeSnippet Format="1.0.0">
    <Header>
      <Title>Modern C++ Header</Title>
      <Shortcut>myheader</Shortcut>
      <Description>Каркас современного заголовочного файла</Description>
      <Author>Developer</Author>
      <SnippetTypes>
        <SnippetType>Expansion</SnippetType>
      </SnippetTypes>
    </Header>
    <Snippet>
      <Declarations>
        <Literal>
          <ID>name</ID>
          <ToolTip>Имя класса</ToolTip>
          <Default>MyClass</Default>
        </Literal>
      </Declarations>
      <Code Language="cpp">
        <![CDATA[#pragma once

class $name$ {
public:
    $name$() = default;
    ~$name$() = default;

    $name$(const $name$&) = default;
    $name$& operator=(const $name$&) = default;

    $name$($name$&&) noexcept = default;
    $name$& operator=($name$&&) noexcept = default;
};
]]>
      </Code>
    </Snippet>
  </CodeSnippet>
</CodeSnippets>
```

### Шаг 2. Куда положить сниппет:
Сохраните файл в стандартную папку пользовательских сниппетов:
`%USERPROFILE%\Documents\Visual Studio 2022\Code Snippets\Visual C++\My Code Snippets\`

### Шаг 3. Использование:
В любом `.hpp` файле наберите **`myheader + Tab + Tab`** — и вы получите готовый класс со всеми методами Правила Пяти!

---

## 5. Автогенерация реализации методов через `Ctrl + .`

Вам **не нужно** вручную копировать сигнатуры из `.h` в `.cpp`:
1. Напишите прототип метода в заголовочном файле:
   ```cpp
   // Player.hpp
   class Player {
   public:
       void TakeDamage(int amount); // Нажали сюда курсором
   };
   ```
2. Нажмите **`Ctrl + .`** (или значок лампочки).
3. Выберите пункт **«Создать определение 'TakeDamage' в Player.cpp»** (Create Definition).
4. Visual Studio автоматически перейдёт в `Player.cpp`, добавит нужный префикс `void Player::TakeDamage(int amount)` и фигурные скобки!
