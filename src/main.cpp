#include <iostream>

#include "thread.h"


class MyThread : public Thread {
public:
	MyThread(int a) : a(a) {}
	MyThread(const std::string &name, int a) :
		Thread(name), a(a) {
	}

	virtual void* run(void *args) {
		int i = this->a;

		for (int n = 1; n < i; ++n)
			std::cout << getName() << " -> " << n <<std::endl;

		return ((void *)1);
	}
private:
	int a;
};

int main() {
	MyThread mt("thread1", 10);
	mt.start();

	MyThread mt2("thread2", 20);
	mt2.start();

	std::cout << "============" <<std::endl;

	return 0;
}
