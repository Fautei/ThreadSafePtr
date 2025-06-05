#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include "thread_safe_ptr.hpp"

struct Counter {
    void increment() {
        ++value;
    }

    int get() const {
        return value;
    }

private:
    int value = 0;
};

int main() {
    using namespace tsp;

    auto counter = thread_safe_ptr<Counter>::make();

    const int num_threads = 10;
    const int increments_per_thread = 1000;

    std::vector<std::thread> threads;

    // Потоки инкрементируют счётчик
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([counter, increments_per_thread]() {
            for (int j = 0; j < increments_per_thread; ++j) {
                counter->increment(); // потокобезопасный вызов
            }
        });
    }

    // Подождать завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }

    // Прочитать значение (тоже потокобезопасно)
    std::cout << "Final counter value: " << counter->get() << "\n";
    // Ожидаемое значение: num_threads * increments_per_thread = 10 * 1000 = 10000

    return 0;
}