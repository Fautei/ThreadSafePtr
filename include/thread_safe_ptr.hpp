#pragma once
#include <memory>
#include <mutex>
#include <utility>

namespace tsp {

template <typename T>
class thread_safe_ptr {
public:
    thread_safe_ptr() = default;

    explicit thread_safe_ptr(std::shared_ptr<T> ptr)
        : _data(std::make_shared<control_block>(std::move(ptr))) {}

    template <typename... Args>
    static thread_safe_ptr make(Args&&... args) {
        return thread_safe_ptr{ std::make_shared<T>(std::forward<Args>(args)...) };
    }
	
	struct control_block {
        explicit control_block(std::shared_ptr<T> p) : ptr(std::move(p)) {}
        std::shared_ptr<T> ptr;
        mutable std::mutex mtx;
    };

    // Scoped access proxy
    class access_proxy {
    public:
        access_proxy(std::shared_ptr<control_block> data)
            : _data(std::move(data)), _lock(_data->mtx) {}

        T* operator->() const noexcept { return _data->ptr.get(); }
        T& operator*() const noexcept { return *(_data->ptr); }

    private:
        std::shared_ptr<control_block> _data;
        std::lock_guard<std::mutex> _lock;
    };

    access_proxy operator->() const noexcept {
        return access_proxy{ _data };
    }

    T& operator*() const noexcept {
        return *(_data->ptr); // Caution: not thread-safe by itself
    }

    std::shared_ptr<T> unsafe_get() const noexcept {
        return _data ? _data->ptr : nullptr;
    }

    explicit operator bool() const noexcept {
        return _data && _data->ptr;
    }

private:


    std::shared_ptr<control_block> _data;
};

} // namespace tsp
