// multi_threaded_shared_ptr.h : включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта.

#pragma once
#include <memory>
#include <atomic>
#include <mutex>


namespace ext {

	template<typename T>
	class concurrent_ptr {
	public:
		using weak_type = std::weak_ptr<T>;
		operator weak_type() = delete;
		using mutex_type = std::mutex;
		//Add all type of constructors
		constexpr concurrent_ptr() noexcept = default;
		constexpr concurrent_ptr(nullptr_t) noexcept {}
		constexpr concurrent_ptr(T* ptr) noexcept { _init(ptr); }
		constexpr concurrent_ptr(const concurrent_ptr& other) noexcept : _ptr{ other._ptr }, _mutex{ other._mutex }, _use_count{ other._use_count } {
			_incref();
		}
		constexpr concurrent_ptr(concurrent_ptr&& other) noexcept : _ptr{ other._ptr }, _mutex{ other._mutex }, _use_count{ other._use_count } {
			other.swap(concurrent_ptr{});
		}

		constexpr ~concurrent_ptr() noexcept {
			_decref();
		}

		constexpr concurrent_ptr& operator=(const concurrent_ptr& other) noexcept {
			if (&other != this) {
				_decref();
				_ptr = other._ptr;
				_mutex = other._mutex;
				_use_count = other._use_count;
				_incref();
			}
			return *this;
		}

		constexpr concurrent_ptr& operator=(concurrent_ptr&& other) noexcept {
			if (&other != this) {
				_decref();
				swap(std::forward<concurrent_ptr>(other));
			}
			return *this;
		}

		template<typename MtxType>
		class proxy {
		public:
			constexpr proxy(const concurrent_ptr* ptr) noexcept : _ptr(ptr), lock(*_ptr->_mutex) {}

			constexpr T* operator->() const noexcept {
				return _ptr->_ptr;
			}

			constexpr ~proxy() noexcept {}

		private:
			const concurrent_ptr* _ptr;
			const std::lock_guard<MtxType> lock;
		};



		template <class Ty, class... Types >
		friend constexpr concurrent_ptr<Ty> make_concurrent(Types&&... _Args);

		constexpr proxy<mutex_type> operator->() const noexcept {
			return proxy<mutex_type>{ this };
		}

		constexpr const T* get() const noexcept {
			return _ptr;
		}

		constexpr const T& operator*() const noexcept {
			return *get();
		}

		constexpr explicit operator bool() const noexcept {
			return get() != nullptr;
		}

		constexpr void reset(T* ptr) noexcept {
			if (_ptr != ptr) {
				_decref();
				_init(ptr);
			}
		}

		constexpr void reset(const concurrent_ptr& ptr = nullptr) noexcept {
			if (&ptr != this && _ptr != ptr._ptr) {
				_decref();
				*this = std::forward<const concurrent_ptr&>(ptr);
			}
		}

		constexpr size_t use_count() const noexcept {
			return _use_count ? *_use_count : 0;
		}

		constexpr void swap(concurrent_ptr&& other) noexcept {
			using std::swap;
			swap(_ptr, other._ptr);
			swap(_mutex, other._mutex);
			swap(_use_count, other._use_count);
		}

	private:
		std::atomic<size_t>* _use_count{ nullptr };
		mutable mutex_type* _mutex{ nullptr };
		mutable T* _ptr{ nullptr };

		constexpr void _decref() noexcept {
			if (_use_count) {
				*_use_count -= 1;
				if (*_use_count == 0) {
					_delete();
				}
			}
		}

		constexpr void _incref() noexcept {
			if (_use_count) {
				*_use_count += 1;
			}
		}

		constexpr void _delete() noexcept {
			delete _use_count;
			delete _mutex;
			delete _ptr;
		}


		constexpr void _init(T* ptr) {
			_use_count = new std::atomic<size_t>(1);
			_mutex = new std::mutex();
			_ptr = ptr;
		}
	};

	template <class T, class... Types>
	constexpr concurrent_ptr<T> make_concurrent(Types&&... args) {
		concurrent_ptr<T> ret;
		ret._init(new T{ std::forward<Types>(args)... });
		return ret;
	}


}

