// multi_threaded_lock_pointer.cpp: определяет точку входа для приложения.
//

#include "multi_threaded_shared_ptr.h"
#include <thread>



struct blocking {
	void method(int tid) {
		cout <<tid<< " start running\n";
		std::this_thread::sleep_for(1000ms);
		cout << tid << " end running\n";
	}
};



int main()
{
	blocking* b = new blocking{};
    locked_ptr<blocking> a = make_locked<blocking>();
	locked_ptr<blocking> a2;
	a2 = a;
	//just for f12 shortcut
	std::shared_ptr<int> s;

	auto t1 = std::thread([a,s]() {
		while (true) {
			cout << "t1 trying to call locking function" << endl;
			a->method(1);
			cout << "t1 released blocking" << endl;
		}
		});
	auto t2 = std::thread([a]() {
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
