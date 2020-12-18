#include <atomic>
#include <mutex>
#include <thread>

#include "selfcountingtype.h"
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

template <class T>
void f(SelfCountingType<T>& x, std::mutex& mutex)
{
	for (int i = 0; i < 100'000; ++i)
	{
		std::lock_guard<std::mutex> lockGuard(mutex);
		++x;
	}
}

int run_atomic()
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

	return x.load();
}

int run_mutex()
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

	return x;
}

auto run_mutex_custom_type()
{
	SelfCountingType<int> x{ 0 };
	std::mutex mutex;
	void (*mutex_custom_type_f)(SelfCountingType<int>&, std::mutex&) = f;

	std::thread t1{ mutex_custom_type_f, std::ref(x), std::ref(mutex) };
	std::thread t2{ mutex_custom_type_f, std::ref(x), std::ref(mutex) };
	std::thread t3{ mutex_custom_type_f, std::ref(x), std::ref(mutex) };
	std::thread t4{ mutex_custom_type_f, std::ref(x), std::ref(mutex) };

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	return x;
}

int main()
{
	std::size_t nb_runs = 50;

	//auto stats = Timer<std::chrono::milliseconds>::time_function_repeatedly(
	//	nb_runs, run_atomic
	//);
	//std::cout << stats << '\n';

	//auto stats2 = Timer<std::chrono::milliseconds>::time_function_repeatedly(
	//	nb_runs, run_mutex
	//);
	//std::cout << stats2 << '\n';

	auto xs = std::vector(nb_runs, 0);
	//xs.assign(nb_runs, 0);

	auto stats3 = Timer<std::chrono::milliseconds>::time_function_repeatedly(
		nb_runs, xs.begin(), run_atomic
	);
	std::cout << stats3;
	if (std::all_of(xs.cbegin(), xs.cend(), [&xs](int i) {return xs[0] == i; }))
		std::cout << "All results are equal to " << xs[0] << "\n\n";

	xs.assign(nb_runs, 0);

	auto stats4 = Timer<std::chrono::milliseconds>::time_function_repeatedly(
		nb_runs, xs.begin(), run_mutex
	);
	std::cout << stats4;
	if (std::all_of(xs.cbegin(), xs.cend(), [&xs](int i) {return xs[0] == i; }))
		std::cout << "All results are equal to " << xs[0] << "\n\n";

	auto xs2 = std::vector(nb_runs, SelfCountingType<int>{});
	std::cout << "\n\n";
	auto stats5 = Timer<std::chrono::milliseconds>::time_function_repeatedly(
		nb_runs, xs2.begin(), run_mutex_custom_type
	);
	std::cout << "\n\n";
	std::cout << stats5;
	if (std::all_of(xs2.cbegin(), xs2.cend(), [&xs2](const SelfCountingType<int>& i) {return xs2[0] == i; }))
		std::cout << "All results are equal to " << xs2[0].value << "\n";
	std::cout << "Custom type number of instances: " << SelfCountingType<int>::instances << '\n';
 
	return 0;
}