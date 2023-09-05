// multi_threaded_lock_pointer.cpp: определяет точку входа для приложения.
//

#include "multi_threaded_shared_ptr.h"
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
using namespace std;
using namespace std::chrono_literals;


struct blocking {
	void method(int tid) {
		cout <<tid<< " start running\n";
		std::this_thread::sleep_for(1000ms);
		cout << tid << " end running\n";
	}
};





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
		constexpr proxy(locked_ptr* ptr) noexcept: _ptr(ptr){
			_ptr->_mutex->lock();
			cout << "locked\n";
		}

		constexpr T* operator->() {
			cout << "ptr returned\n";
			return _ptr->_ptr;
		}

		constexpr ~proxy() {
			cout << "unlocked\n";
			_ptr->_mutex->unlock();
			
		}

	private:
		locked_ptr* _ptr;
	};

	template <class Ty, class... Types >
	friend constexpr  locked_ptr<Ty> make_locked(Types&&... _Args);

	constexpr proxy operator->() noexcept {
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

int main()
{
	blocking* b = new blocking{};
    locked_ptr<blocking> a = make_locked<blocking>();
	locked_ptr<blocking> a2;
	a2 = a;
	//just for f12 shortcut
	std::shared_ptr<int> s;

	auto t1 = std::thread([&a]() {
		while (true) {
			cout << "t1 trying to call locking function" << endl;
			a->method(1);
			cout << "t1 released blocking" << endl;
		}
		});
	auto t2 = std::thread([&a]() {
		while (true) {
			cout << "t2 trying to call locking function" << endl;
			a->method(2);
			cout << "t2 released blocking" << endl;
		}
		});
    std::make_shared<int>(0);
	t1.join();
	t2.join();

	cout << "Hello CMake." << endl;
	return 0;
}
