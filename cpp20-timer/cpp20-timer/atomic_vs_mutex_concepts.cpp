#include <atomic>
#include <mutex>
#include <thread>

#include "instancecountingtype.h"
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
void f(InstanceCountingType<T>& x, std::mutex& mutex)
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
	InstanceCountingType<int> x{ 0 };
	std::mutex mutex;
	void (*mutex_custom_type_f)(InstanceCountingType<int>&, std::mutex&) = f;

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
	// Higher order function
	auto equal_to_first_pred = []<class Container>(const Container& c)
	{
		return [&front = std::as_const(c.front())](typename Container::const_reference elem) 
		{
			return front == elem;
		};
	};

	std::size_t nb_runs = 50u;

	//////////// Atomic test - no return values
	{
		auto stats = Timer<std::chrono::milliseconds>::time_function_repeatedly(
			nb_runs, run_atomic
		);
		std::cout << stats << '\n';
	}
	//////////// Mutex test - no return values
	{
		auto stats = Timer<std::chrono::milliseconds>::time_function_repeatedly(
			nb_runs, run_mutex
		);
		std::cout << stats << '\n';
	}
	//////////// Atomic test - with return values
	{
		auto results = std::vector(nb_runs, 0);

		auto stats = Timer<std::chrono::milliseconds>::time_function_repeatedly(
			nb_runs, results.begin(), run_atomic
		);
		std::cout << stats;

		bool all_are_equal = std::all_of(results.cbegin(), results.cend(),
										 equal_to_first_pred(results));
		if (all_are_equal)
			std::cout << "All results are equal to " << results.front() << "\n\n";
	}
	//////////// Mutex test - with return values
	{
		auto results = std::vector(nb_runs, 0);

		auto stats = Timer<std::chrono::milliseconds>::time_function_repeatedly(
			nb_runs, results.begin(), run_mutex
		);
		std::cout << stats;

		bool all_are_equal = std::all_of(results.cbegin(), results.cend(),
									     equal_to_first_pred(results));
		if (all_are_equal)
			std::cout << "All results are equal to " << results.front() << "\n\n";
	}
	//////////// Mutex test - with return values && custom type
	{
		auto results = std::vector(nb_runs, InstanceCountingType<int>{});
		std::cout << "\n\n";

		auto stats = Timer<std::chrono::milliseconds>::time_function_repeatedly(
			nb_runs, results.begin(), run_mutex_custom_type
		);
		std::cout << "\n\n" << stats;

		bool all_are_equal = std::all_of(results.cbegin(), results.cend(), 
										 equal_to_first_pred(results));
		if (all_are_equal)
			std::cout << "All results are equal to " << results.front().value << '\n';

		std::cout << "Custom type number of instances: " << InstanceCountingType<int>::instances << '\n';
	}

	return 0;
}