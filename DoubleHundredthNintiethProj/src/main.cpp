#include"SelfTP.h"

void test(void* arg) {
	auto a = std::this_thread::get_id();

	std::cout << "cur  threadID:" << a << " 当前数值：" << *(int*)arg << std::endl;
}

int main() {

	ThreadPool tp(4, 10);

	for (int i = 0; i < 100; i++) {
		tp.PushJob(test, &i, 4);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	return 0;
}