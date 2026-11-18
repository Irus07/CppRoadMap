# 01. Руководство по Modern CMake: Сборка проектов без боли

> В C# у вас есть файл `.csproj` и команда `dotnet build`. В C++ главным стандартом де-факто для описания сборки является **CMake**. 

---

## 1. Что такое CMake на самом деле?

CMake — это **мета-генератор сборок**. Он сам не компилирует код!
Он читает файл `CMakeLists.txt` и генерирует нативные файлы сборки для вашей ОС:
* На Windows с Visual Studio — генерирует решение `.sln` и проекты `.vcxproj`.
* С Ninja / Make — генерирует файлы `build.ninja` или `Makefile`.

```
CMakeLists.txt 
      │
      ▼ (cmake -B build)
[Ninja / MSBuild]
      │
      ▼ (cmake --build build)
[Исполняемый файл .exe]
```

---

## 2. Эталонный современный `CMakeLists.txt` (Target-Based)

Забудьте старые туториалы из 2010 года с `include_directories()` и `add_definitions()`. В современном CMake всё строится вокруг концепции **Targets (Целей)**.

```cmake
cmake_minimum_required(VERSION 3.20)

# Имя проекта, версия и используемые языки:
project(CppRoadmapTutorial VERSION 1.0.0 LANGUAGES CXX)

# Включаем стандарт C++20 для всего проекта:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Создаем исполняемый файл:
add_executable(MyAwesomeApp
    src/main.cpp
    src/utils.cpp
)

# Указываем папки с заголовочными файлами для этой конкретной цели:
target_include_directories(MyAwesomeApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Включаем строгие предупреждения компилятора:
if (MSVC)
    target_compile_options(MyAwesomeApp PRIVATE /W4 /permissive-)
else()
    target_compile_options(MyAwesomeApp PRIVATE -Wall -Wextra -Wpedantic)
endif()
```

---

## 3. Команды для сборки из терминала (PowerShell / CMD)

Три универсальные команды, которые работают везде:

```powershell
# 1. Конфигурация (создает папку build и генерирует файлы сборки):
cmake -B build -S .

# 2. Компиляция проекта (автоматически вызывает MSVC или Ninja):
cmake --build build --config Release

# 3. Запуск получившегося бинарника:
.\build\Release\MyAwesomeApp.exe
```

---

## 4. Подключение внешних библиотек через `FetchContent`

Самый простой способ подключить современную библиотеку без установки сторонних менеджеров пакетов — модуль `FetchContent` (CMake сам скачает её с GitHub и скомпилирует вместе с вашим проектом):

```cmake
include(FetchContent)

# Подключаем быструю библиотеку форматирования строк fmt:
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        10.2.1
)
FetchContent_MakeAvailable(fmt)

# Линкуем библиотеку к нашей программе:
target_link_libraries(MyAwesomeApp PRIVATE fmt::fmt)
```
