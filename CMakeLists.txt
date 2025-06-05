cmake_minimum_required(VERSION 3.16)

project(multi_threaded_shared_ptr VERSION 0.1 LANGUAGES CXX)

# Настройка компилятора
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Горячая перезагрузка для MSVC (опционально)
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
    "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,
    $<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,
    $<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Основная библиотека (заголовочная, header-only)
add_library(mts INTERFACE)
target_include_directories(mts INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Примеры
option(BUILD_EXAMPLES "Build examples" ON)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Тесты
include(CTest)
option(BUILD_TESTING "Build tests" ON)
if (BUILD_TESTING)
    add_subdirectory(tests)
endif()