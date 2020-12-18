#pragma once

#include <chrono>
#include <cmath>
#include <concepts>
#include <functional>
#include <iostream>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>


template<class T>
struct is_duration : std::false_type {};

template<class Rep, class Period>
struct is_duration<std::chrono::duration<Rep, Period>> : std::true_type {};

template <class T>
inline constexpr bool is_duration_v = is_duration<T>::value;

template <class T>
concept duration = is_duration_v<T>;


template <duration D = std::chrono::nanoseconds>
class Timer
{
public:

	template <duration D = std::chrono::nanoseconds>
	struct Statistics
	{
		double mean;
		double variance;
		double standard_deviation;
		std::vector<D> durations;

		explicit Statistics(std::vector<D>&& times)
			: durations{ times }
		{
			double sum = std::accumulate(
				durations.cbegin(), 
				durations.cend(), 
				0.0,
				[]<duration D>(double&& sum, const D& time)
				{
					return sum + static_cast<double>(time.count());
				}
			);
			mean = sum / durations.size();

			double variances_sum = std::accumulate(
				durations.cbegin(),
				durations.cend(),
				0.0,
				[&mean = std::as_const(mean)]<duration D>(double&& sum, const D& time)
				{
					return sum + std::pow(static_cast<double>(time.count()) - mean, 2);
				}
			);
			variance = variances_sum / durations.size();

			standard_deviation = std::sqrt(variance);
		}

		friend std::ostream& operator<< (std::ostream& out, const Statistics<D>& stats)
		{
			out << "Mean: " << stats.mean << '\n'
				<< "Variance: " << stats.variance << '\n'
				<< "Standard deviation: " << stats.standard_deviation << '\n';
			
			out << "Durations: ";
			size_t size = std::min<size_t>(5, stats.durations.size());
			for (size_t i = 0; i < size; ++i)
			{
				out << stats.durations[i].count() << ' ';
			}
			return out << "...\n";
		}
	};

	template <class Function, class... Args>
	requires std::invocable<Function, Args...>
	static
	auto time_function(Function&& f, Args&&... args) -> D
	{
		auto start = std::chrono::high_resolution_clock::now();
		if constexpr (std::is_same_v<std::invoke_result_t<Function, Args...>, void>)
		{
			std::invoke(std::forward<Function>(f), std::forward<Args>(args)...);
		}
		else
		{
			auto result = std::invoke(
				std::forward<Function>(f), 
				std::forward<Args>(args)...
			);
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto time = end - start;
		return std::chrono::duration_cast<D>(time);
	}

	template <class Function, class... Args>
	requires std::invocable<Function, Args...>
	static
	auto time_function_repeatedly(size_t repeats, Function&& f, Args&&... args) -> Statistics<D>
	{
		std::vector<D> results;
		results.reserve(repeats);
		for (size_t i = 0; i < repeats; ++i)
		{
			auto&& time = Timer::time_function(std::forward<Function>(f), std::forward<Args>(args)...);
			results.emplace_back(time);
		}
		return Statistics<D>(std::move(results));
	}
};
