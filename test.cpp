// multi_threaded_lock_pointer.cpp: определяет точку входа для приложения.
//

#include "include/multi_threaded_shared_ptr.hpp"
#include <thread>
#include <cassert>
#include <iostream>

using namespace std;
using namespace std::chrono_literals;
using namespace ext;

struct mutable_object {
	void method(const std::string& msg) {
		std::cout <<"method: " << msg << endl;
		mutable_state = 10;
		std::this_thread::sleep_for(100ms);
		assert(mutable_state == 10);
		cout << boolalpha << (mutable_state == 10);
	}

	void method2(const std::string& msg) {
		std::cout << "method2: " << msg << endl;
		mutable_state = 20;
		std::this_thread::sleep_for(100ms);
		assert(mutable_state == 20);
		cout << boolalpha << (mutable_state == 20);
	}

	void method3(const std::string& msg) {
		std::cout << "method3: " << msg << endl;
		mutable_state = 50;
		assert(mutable_state == 50);
		std::this_thread::sleep_for(100ms);
		assert(mutable_state == 50);
		cout << boolalpha << (mutable_state == 50);
		method(msg);
		method2(msg);
	}

	int mutable_state;
};



int main()
{
    locked_ptr<mutable_object> a = make_locked<mutable_object>();
	locked_ptr<mutable_object> a2 = a;
	a = a;
	a2.reset(a);
	a2.reset(a.get());
	a2.reset();
	//just for f12 shortcut
	std::shared_ptr<int> s;

	auto t1 = std::thread([a]() {
		for (int i = 0; i < 50;i++) {
			a->method("executing in t1");
			std::this_thread::sleep_for(1000ms);
		}
		});
	auto t2 = std::thread([a]() {
		for (int i = 0; i < 50;i++) {
			auto a2 = a;
			a2->method3("executing in t2");
			std::this_thread::sleep_for(100ms);
		}
		});

	auto p2 = a;
    
	for (int i = 0; i < 50; i++) {
		
		p2->method2("executing in main");
		std::this_thread::sleep_for(500ms);
	}

	t1.join();
	t2.join();

	cout << "Hello CMake." << endl;
	return 0;
}
