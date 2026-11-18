# 02. Санитайзеры, отладка и управление зависимостями

> В C# среда выполнения защищает вас от ошибок памяти. В C++ вашими лучшими защитниками являются **санитайзеры компилятора**. Они превращают 99% скрытых ошибок памяти в мгновенные и понятные отчёты с номерами строк.

---

## 1. Санитайзеры (Sanitizers): Ваш щит от багов

Санитайзеры встраивают проверки прямо в исполняемый машинный код на этапе компиляции:

| Санитайзер | Что ловит | Флаг компилятора |
| :--- | :--- | :--- |
| **AddressSanitizer (ASan)** | Выход за границы массива, Use-after-free, Use-after-scope, двойное освобождение | `-fsanitize=address` (или `/fsanitize=address` в MSVC) |
| **UndefinedBehaviorSanitizer (UBSan)** | Знаковое переполнение `int`, сдвиги на отрицательные числа, разыменование nullptr | `-fsanitize=undefined` |
| **ThreadSanitizer (TSan)** | Состояния гонки данных (Data Races) между потоками | `-fsanitize=thread` |

### Как включить AddressSanitizer в CMake:
```cmake
option(ENABLE_ASAN "Enable Address Sanitizer" ON)

if (ENABLE_ASAN)
    if (MSVC)
        target_compile_options(MyTarget PRIVATE /fsanitize=address)
    else()
        target_compile_options(MyTarget PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(MyTarget PRIVATE -fsanitize=address,undefined)
    endif()
endif()
```

Если в коде произойдёт обращение к удалённой памяти:
```cpp
int* p = new int[5];
delete[] p;
std::cout << p[0]; // USE-AFTER-FREE!
```
Программа не упадёт молча, а напечатает детальный стектрейс:
`ERROR: AddressSanitizer: heap-use-after-free on address 0x... at line 12`.

---

## 2. Менеджер пакетов `vcpkg` (NuGet для мира C++)

Если вы привыкли к `dotnet add package`, вашим выбором в C++ будет **vcpkg** от Microsoft.

### Быстрый старт:
```powershell
# 1. Клонируем репозиторий vcpkg:
git clone https://github.com/microsoft/vcpkg.git c:\tools\vcpkg
cd c:\tools\vcpkg
.\bootstrap-vcpkg.bat

# 2. Интеграция с Visual Studio и CMake (глобально):
.\vcpkg integrate install
```

### Использование с манифестом (`vcpkg.json` — аналог `.csproj`):
Создайте рядом с `CMakeLists.txt` файл `vcpkg.json`:
```json
{
  "name": "my-cpp-project",
  "version-string": "1.0.0",
  "dependencies": [
    "nlohmann-json",
    "fmt",
    "catch2"
  ]
}
```
CMake автоматически загрузит и подключит зависимости при конфигурации проекта!

---

## 3. Статический анализ с `clang-tidy`

`clang-tidy` — это как ReSharper / Roslyn-анализаторы в C#. Он анализирует абстрактное синтаксическое дерево (AST) и подсказывает, где можно применить `std::move`, где пропущен `const`, а где закралась утечка ресурсов.
Настройте файл `.clang-tidy` в корне репозитория, чтобы поддерживать современный стиль кода.
