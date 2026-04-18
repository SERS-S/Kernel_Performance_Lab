## Дерево директорий

```text
kpl/
├── README.md
├── CMakeLists.txt
├── app/
│   └── main.cpp
├── core/
│   ├── include/
│   │   ├── run_context.hpp
│   │   ├── i_stage.hpp
│   │   └── run_service.hpp
│   └── src/
│       ├── run_service.cpp
│       ├── stages.cpp
│       └── json_api.cpp
├── ffi/
│   ├── kpl_backend_api.h
│   ├── kpl_backend_api.cpp
│   └── kpl_frontend.h
├── frontend/
│   ├── Cargo.toml
│   └── src/
│       ├── lib.rs
│       ├── app.rs
│       └── backend_api.rs
├── scenarios/
│   └── fio_smoke.json
└── runs/
    └── .gitkeep
```

## Что должно быть в каждой директории и файле

### `/CMakeLists.txt`
- сборка C++ executable
- сборка `core`
- сборка C++ FFI/adaptor layer
- вызов `cargo build` для Rust frontend library или подключение уже собранной Rust-библиотеки
- подключение include-путей
- линковка зависимостей
- линковка Rust frontend как `staticlib` для единого исполняемого файла или как `cdylib`/shared library
- базовый output path для executable и `.a/.lib/.so/.dll/.dylib`

---

## `/app`
Запускающий слой приложения.

#### `main.cpp`
Должен содержать:
- единственный `main()` проекта
- определение workspace/root paths
- создание `RunService`
- создание таблицы backend API для Rust frontend
- запуск Rust UI через экспортированную функцию `kpl_frontend_run(...)`
- корректное завершение приложения

---

## `/core`
Главная бизнес-логика.

### `/core/include`
Только основные интерфейсы и общие структуры.

#### `run_context.hpp`
Должен содержать:
- структуру данных, которая проходит через pipeline
- scenario/config
- environment snapshot
- raw artifacts
- parsed metrics
- status/error
- путь к run directory

#### `i_stage.hpp`
Должен содержать:
- интерфейс одного шага pipeline
- что-то вроде `execute(RunContext&)`

#### `run_service.hpp`
Должен содержать:
- orchestration API
- `start_run`
- `get_run`
- `list_runs`
- `compare_runs` (можно заглушкой на старте)

---

### `/core/src`
Реализация core.

#### `run_service.cpp`
Должен содержать:
- сборку pipeline из stages
- запуск шагов по порядку
- управление статусами `queued/running/success/failed/partial_success`

#### `stages.cpp`
Должен содержать:
- минимальные стадии:
  - preflight
  - env capture
  - benchmark run
  - parse summary
  - persist result
- можно держать все стадии в одном файле

#### `json_api.cpp`
Должен содержать:
- сериализацию `RunContext` / summary / run list в JSON
- подготовку данных для FFI-границы

---

## `/ffi`
Тонкий слой между C++ executable и Rust frontend library.

#### `kpl_backend_api.h`
Должен содержать:
- C-compatible таблицу function pointers для backend API
- opaque `void* ctx`, указывающий на C++ `RunService`
- функции вида:
  - run_start
  - run_status
  - run_list
  - run_get
  - run_compare
  - string_free
- только примитивы, `const char*`, `char**`, JSON-строки

#### `kpl_backend_api.cpp`
Должен содержать:
- адаптацию вызовов из Rust UI в C++ `RunService`
- преобразование ошибок в строки
- выделение/освобождение памяти под JSON-ответы
- фабрику таблицы backend API для `main.cpp`

#### `kpl_frontend.h`
Должен содержать:
- объявление Rust-exported функции запуска UI
- что-то вроде `extern "C" int kpl_frontend_run(const KplBackendApi* api)`

---

## `/frontend`
Rust UI, компилируемый как библиотека, а не как самостоятельное приложение.

#### `Cargo.toml`
Должен содержать:
- секцию `[lib]`
- `crate-type = ["staticlib"]` для единого исполняемого файла или `crate-type = ["cdylib"]` для shared library
- зависимости:
  - `eframe` / `egui`
  - `serde`
  - `serde_json`
- настройки сборки Rust frontend library

#### `/frontend/src/lib.rs`
Должен содержать:
- exported C ABI функцию запуска frontend, например `kpl_frontend_run`
- прием `KplBackendApi`
- запуск `App`

#### `/frontend/src/app.rs`
Должен содержать:
- UI-экран списка прогонов
- UI-экран деталей прогона
- кнопки refresh/open/start
- минимальный state приложения

#### `/frontend/src/backend_api.rs`
Должен содержать:
- Rust-представление C-compatible `KplBackendApi`
- вызовы backend через function pointers
- safe wrapper поверх переданной backend API table
- десериализацию JSON в Rust-структуры

---

## `/scenarios`
Конфиги сценариев запуска.

#### `fio_smoke.json`
Должен содержать:
- имя сценария
- команду/параметры benchmark
- timeout
- опции профилирования
- минимальную smoke-конфигурацию

---

## `/runs`
Каталог с результатами запусков.
