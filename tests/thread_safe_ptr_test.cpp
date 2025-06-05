#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include "../include/thread_safe_ptr.hpp"

using namespace tsp;

struct Dummy {
    Dummy(int val = 0) : value(val) {}
    void set(int v) { value = v; }
    int get() const { return value; }
    int value;
};

// Проверка базовой инициализации и доступа
TEST(ThreadSafePtrTest, BasicAccess) {
    auto ptr = thread_safe_ptr<Dummy>::make(42);
    ASSERT_TRUE(ptr);
    EXPECT_EQ(ptr->get(), 42);
    ptr->set(99);
    EXPECT_EQ(ptr->get(), 99);
}

// Проверка копирования
TEST(ThreadSafePtrTest, CopySemantics) {
    auto a = thread_safe_ptr<Dummy>::make(10);
    auto b = a;
    b->set(20);
    EXPECT_EQ(a->get(), 20);
    EXPECT_EQ(b->get(), 20);
}

// Проверка перемещения
TEST(ThreadSafePtrTest, MoveSemantics) {
    auto a = thread_safe_ptr<Dummy>::make(55);
    auto b = std::move(a);
    EXPECT_TRUE(b);
    EXPECT_FALSE(static_cast<bool>(a));
    EXPECT_EQ(b->get(), 55);
}

// Проверка работы в многопоточном контексте
TEST(ThreadSafePtrTest, MultithreadedIncrement) {
    struct Counter {
        void inc() { ++val; }
        int get() const { return val; }
        int val = 0;
    };

    auto ptr = thread_safe_ptr<Counter>::make();

    const int num_threads = 8;
    const int per_thread = 1000;

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([ptr, per_thread]() {
            for (int j = 0; j < per_thread; ++j) {
                ptr->inc();
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(ptr->get(), num_threads * per_thread);
}
