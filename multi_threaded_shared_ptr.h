// multi_threaded_shared_ptr.h : включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта.

#pragma once

#include <iostream>
#include <memory>
#include <atomic>

#include <mutex>
using namespace std;
using namespace std::chrono_literals;


template<typename T>
class locked_ptr {
public:
	using weak_type = weak_ptr<T>;
	operator weak_type() = delete;

	//Add all type of constructors
	constexpr locked_ptr() noexcept = default;
	constexpr locked_ptr(nullptr_t) noexcept {}
	constexpr locked_ptr(T* ptr) noexcept { _init(ptr); }
	constexpr locked_ptr(const locked_ptr<T>& other) noexcept {
		_ptr = other._ptr;
		_mutex = other._mutex;
		_use_count = other._use_count;
		_incref();
	}
	constexpr locked_ptr(locked_ptr<T>&& other) noexcept {
		swap(_ptr, other._ptr);
		swap(_mutex, other._mutex);
		swap(_use_count, other._use_count);
	}

	constexpr ~locked_ptr() noexcept {
		_decref();
	}

	locked_ptr& operator=(const locked_ptr& other) noexcept {
		if (&other == this) {
			return *this;
		}
		_ptr = other._ptr;
		_mutex = other._mutex;
		_use_count = other._use_count;
		_incref();
		return *this;
	}

	locked_ptr& operator=(locked_ptr&& other) noexcept {
		if (&other == this) {
			return *this;
		}
		swap(_ptr, other._ptr);
		swap(_mutex, other._mutex);
		swap(_use_count, other._use_count);
		return *this;
	}

	class proxy {
	public:
		constexpr proxy(const locked_ptr* ptr) noexcept : _ptr(ptr) {
			_ptr->_mutex->lock();
			cout << "locked\n";
		}

		constexpr T* operator->() const noexcept {
			cout << "ptr returned\n";
			return _ptr->_ptr;
		}

		constexpr ~proxy() noexcept {
			cout << "unlocked\n";
			_ptr->_mutex->unlock();

		}

	private:
		const locked_ptr* _ptr;
	};

	template <class Ty, class... Types >
	friend constexpr  locked_ptr<Ty> make_locked(Types&&... _Args);

	constexpr proxy operator->() const noexcept {
		return proxy{ this };
	}

	constexpr T* get() const noexcept {
		return _ptr;
	}

	constexpr T& operator*() const noexcept {
		return *get();
	}

	constexpr explicit operator bool() const noexcept {
		return get() != nullptr;
	}



private:
	std::atomic<size_t>* _use_count{ nullptr };
	mutable std::mutex* _mutex{ nullptr };
	mutable T* _ptr{ nullptr };

	constexpr void _decref() noexcept {
		if (!_use_count) {
			return;
		}
		*_use_count -= 1;
		if (*_use_count == 0) {
			_delete();
		}
	}

	constexpr void _incref() noexcept {
		*_use_count += 1;
	}

	constexpr void _delete() noexcept {
		if (_use_count) {
			delete _use_count;
		}
		if (_mutex) {
			delete _mutex;
		}
		if (_ptr) {
			delete _ptr;
		}
	}

	constexpr void _init(T* ptr) {
		_use_count = new atomic<size_t>(1);
		_mutex = new mutex();
		_ptr = ptr;
	}
};

template <class T, class... Types>
constexpr locked_ptr<T> make_locked(Types&&... args) {
	locked_ptr<T> ret;
	ret._init(new T{ std::forward<Types>(args)... });
	return ret; // _Ret;
}