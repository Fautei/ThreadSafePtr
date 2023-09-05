// multi_threaded_lock_pointer.cpp: определяет точку входа для приложения.
//

#include "multi_threaded_shared_ptr.h"
#include <thread>
#include <cassert>



struct mutable_object {
	void method() {
		mutable_state = 10;
		std::this_thread::sleep_for(1000ms);
		assert(mutable_state == 10);
	}

	void othermethod() {
		mutable_state = 50;
		std::this_thread::sleep_for(500ms);
		assert(mutable_state == 50);
	}

	int mutable_state = 0;
};



int main()
{
	mutable_object* b = new mutable_object{};
    locked_ptr<mutable_object> a = make_locked<mutable_object>();
	locked_ptr<mutable_object> a2 = a;
	a = a;
	//just for f12 shortcut
	std::shared_ptr<int> s;

	auto t1 = std::thread([a,s]() {
		while (true) {
			cout << "t1 trying to call locking function" << endl;
			a->method();
			cout << "t1 released blocking" << endl;
		}
		});
	auto t2 = std::thread([a]() {
		while (true) {
			cout << "t2 trying to call locking function" << endl;
			a->method();
			cout << "t2 released blocking" << endl;
		}
		});
    
	while (true) {
		cout << "main trying to call locking function" << endl;
		a->othermethod();
		cout << "main released blocking" << endl;
	}

	t1.join();
	t2.join();

	cout << "Hello CMake." << endl;
	return 0;
}
