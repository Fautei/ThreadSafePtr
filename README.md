# Execution-Around Pointer for Thread-Safe Access
thread_safe_ptr is a lightweight, header-only smart pointer that enables automatic mutex locking for safe concurrent access to shared objects in C++.

It wraps your object with a std::shared_ptr and synchronizes access via internal std::mutex, using the Execution Around Pointer idiom.

✅ Features
Thread-safe access with automatic locking (operator->)

Shared ownership (shared_ptr-like semantics)

Header-only & C++20-compatible

Clean RAII design

🔧 Example
```cpp
#include <mts/thread_safe_ptr.hpp>

struct Counter {
    void inc() { ++value; }
    int get() const { return value; }
    int value = 0;
};

int main() {
    auto counter = mts::thread_safe_ptr<Counter>::make();

    counter->inc(); // safely increments
    int value = counter->get(); // safely reads
}
```

🚀 Multithreaded Usage

```cpp
#include <thread>
#include <vector>

auto counter = mts::thread_safe_ptr<Counter>::make();

std::vector<std::thread> threads;
for (int i = 0; i < 4; ++i) {
    threads.emplace_back([counter]() {
        for (int j = 0; j < 1000; ++j) {
            counter->inc(); // auto-locked
        }
    });
}
for (auto& t : threads) t.join();

std::cout << "Total: " << counter->get() << "\n";
```

🧪 Tests
To build and run tests:

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
cd build && ctest
📁 Project Structure
```

```css
include/
└── mts/
    └── thread_safe_ptr.hpp   ← main header

examples/
└── example_thread_safe_ptr.cpp

tests/
└── thread_safe_ptr_test.cpp
```

📜 License
This project is licensed under the MIT License.

