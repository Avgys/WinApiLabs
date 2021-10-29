#include <iostream>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

int main() {
	const char str[] = "stroka";
	std::cout << (std::uint64_t)&str << std::endl;
	for (;;)
	{		
		std::this_thread::sleep_for(1s);
		std::cout << str << std::endl;
	}
	return 0;
}