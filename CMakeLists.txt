cmake_minimum_required(VERSION 3.24)

project(
    KernelPerformanceLab
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

option(KPL_ENABLE_WARNINGS "Enable compiler warnings" ON)
option(KPL_BUILD_RUST_FRONTEND "Build and link the Rust frontend library" OFF)

add_library(kpl_core STATIC
    core/src/run_service.cpp
    core/src/stages.cpp
    core/src/json_api.cpp
)

target_include_directories(kpl_core
    PUBLIC
        "${PROJECT_SOURCE_DIR}/core/include"
)

target_compile_features(kpl_core PUBLIC cxx_std_20)

add_library(kpl_backend_api STATIC
    ffi/kpl_backend_api.cpp
)

target_include_directories(kpl_backend_api
    PUBLIC
        "${PROJECT_SOURCE_DIR}/ffi"
        "${PROJECT_SOURCE_DIR}/core/include"
)

target_link_libraries(kpl_backend_api
    PUBLIC
        kpl_core
)

target_compile_features(kpl_backend_api PUBLIC cxx_std_20)

add_executable(kpl
    app/main.cpp
)

target_include_directories(kpl
    PRIVATE
        "${PROJECT_SOURCE_DIR}/ffi"
        "${PROJECT_SOURCE_DIR}/core/include"
)

target_link_libraries(kpl
    PRIVATE
        kpl_backend_api
        kpl_core
)

if(KPL_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(kpl_core PRIVATE /W4)
        target_compile_options(kpl_backend_api PRIVATE /W4)
        target_compile_options(kpl PRIVATE /W4)
    else()
        target_compile_options(kpl_core PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(kpl_backend_api PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(kpl PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

if(KPL_BUILD_RUST_FRONTEND)
    find_program(CARGO_EXECUTABLE cargo REQUIRED)

    set(KPL_RUST_PROFILE "debug")
    set(KPL_RUST_TARGET_DIR "${CMAKE_BINARY_DIR}/cargo")
    set(KPL_RUST_FRONTEND_DIR "${PROJECT_SOURCE_DIR}/frontend")

    add_custom_target(kpl_frontend_rust ALL
        COMMAND
            "${CMAKE_COMMAND}" -E env
            "CARGO_TARGET_DIR=${KPL_RUST_TARGET_DIR}"
            "${CARGO_EXECUTABLE}" build
        WORKING_DIRECTORY "${KPL_RUST_FRONTEND_DIR}"
        COMMENT "Building Rust frontend library"
        VERBATIM
    )

    add_dependencies(kpl kpl_frontend_rust)
endif()
