#include <atomic>
#include <mutex>
#include <thread>

#include "timing.h"

void f(std::atomic<int>& x)
{
	for (int i = 0; i < 100'000; ++i)
	{
		++x;
	}
}

void f(int& x, std::mutex& mutex)
{
	for (int i = 0; i < 100'000; ++i)
	{
		std::lock_guard<std::mutex> lockGuard(mutex);
		++x;
	}
}

void run_atomic()
{
	std::atomic_int x = 0;
	void (*atomic_f)(std::atomic_int&) = f;

	// std::thread t{ f, std::ref(x) };  // compiler error - ambiguous call

	//std::thread t1{ static_cast<void(*)(std::atomic_int&)>(f), std::ref(x) };
	std::thread t1{ atomic_f, std::ref(x) };
	std::thread t2{ atomic_f, std::ref(x) };
	std::thread t3{ atomic_f, std::ref(x) };
	std::thread t4{ atomic_f, std::ref(x) };

	t1.join();
	t2.join();
	t3.join();
	t4.join();
}

void run_mutex()
{
	int x = 0;
	std::mutex mutex;
	void (*mutex_f)(int&, std::mutex&) = f;

	std::thread t1{ mutex_f, std::ref(x), std::ref(mutex) };
	std::thread t2{ mutex_f, std::ref(x), std::ref(mutex) };
	std::thread t3{ mutex_f, std::ref(x), std::ref(mutex) };
	std::thread t4{ mutex_f, std::ref(x), std::ref(mutex) };

	t1.join();
	t2.join();
	t3.join();
	t4.join();
}

int main()
{
	int nb_runs = 50;

	auto stats = Timer<std::chrono::milliseconds>::time_function_repeatedly(
		nb_runs, run_atomic
	);
	std::cout << stats << '\n';

	auto stats2 = Timer<std::chrono::milliseconds>::time_function_repeatedly(
		nb_runs, run_mutex
	);
	std::cout << stats2;

	return 0;
}