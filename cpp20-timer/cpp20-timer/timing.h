#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <concepts>
#include <functional>
#include <iostream>
#include <iterator>
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


template <duration Duration = std::chrono::nanoseconds>
class Timer
{
public:

	template <duration Duration = std::chrono::nanoseconds>
	struct Statistics
	{
		double mean;
		double variance;
		double standard_deviation;
		std::vector<Duration> durations;

		explicit Statistics(std::vector<Duration>&& times)
			: durations{ times }
		{
			double sum = std::accumulate(
				durations.cbegin(),
				durations.cend(),
				0.0,
				[]<duration Duration>(double&& sum, const Duration& time)
				{
					return sum + static_cast<double>(time.count());
				}
			);
			mean = sum / durations.size();

			double variances_sum = std::accumulate(
				durations.cbegin(),
				durations.cend(),
				0.0,
				[&mean = std::as_const(mean)]<duration Duration>(double&& sum, const Duration& time)
				{
					return sum + std::pow(static_cast<double>(time.count()) - mean, 2);
				}
			);
			variance = variances_sum / durations.size();

			standard_deviation = std::sqrt(variance);
		}

		friend std::ostream& operator<< (std::ostream& out, const Statistics<Duration>& stats)
		{
			out << "Mean: " << stats.mean << '\n'
				<< "Variance: " << stats.variance << '\n'
				<< "Standard deviation: " << stats.standard_deviation << '\n';

			out << "Durations: ";
			std::size_t size = std::min<std::size_t>(5, stats.durations.size());
			for (std::size_t i = 0; i < size; ++i)
			{
				out << stats.durations[i].count() << ' ';
			}
			return out << "...\n";
		}
	};

	template <class Function, class... Args>
	requires std::invocable<Function, Args...>
		  && std::is_assignable_v<std::invoke_result_t<Function, Args...>,
							      std::invoke_result_t<Function, Args...>>
	[[nodiscard]] static auto time_function(Function&& f, Args&&... args) -> Duration
	{
		auto start = std::chrono::high_resolution_clock::now();
		if constexpr (std::is_same_v<std::invoke_result_t<Function, Args...>, void>)
		{
			std::invoke(std::forward<Function>(f), std::forward<Args>(args)...);
		}
		else
		{
			auto result = std::invoke(std::forward<Function>(f),
									  std::forward<Args>(args)...);
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto time = end - start;
		return std::chrono::duration_cast<Duration>(time);
	}

	template <class Function, class... Args>
	requires std::invocable<Function, Args...> 
		  && std::is_assignable_v<std::invoke_result_t<Function, Args...>&,
								  std::invoke_result_t<Function, Args...>>
	[[nodiscard]] static auto time_function(
		std::invoke_result_t<Function, Args...>& result, 
		Function&& f, Args&&... args) -> Duration
	{
		auto start = std::chrono::high_resolution_clock::now();
		result = std::invoke(std::forward<Function>(f), std::forward<Args>(args)...);
		auto end = std::chrono::high_resolution_clock::now();
		auto time = end - start;
		return std::chrono::duration_cast<Duration>(time);
	}

	template <class Function, class... Args>
	requires std::invocable<Function, Args...>
	[[nodiscard]] static auto time_function_repeatedly(
		std::size_t repeats, Function&& f, Args&&... args) -> Statistics<Duration>
	{
		std::vector<Duration> results;
		results.reserve(repeats);
		std::generate_n(std::back_inserter(results), repeats,
			// Here are various ways in which i tried perfect forwarding in a lambda:
			// works:
			[&f, &args...]
			// works:
			//[&f, ... args = std::forward<Args>(args)]
			// compiler error if I also forward f in the body:
			//[f = std::forward<Function>(f), &args...]
			// compiler error if I also forward f in the body:
			//[f = std::forward<Function>(f), ... args = std::forward<Args>(args)]
			{
				return Timer::time_function(
					std::forward<Function>(f), std::forward<Args>(args)...);
			}
		);
		return Statistics<Duration>(std::move(results));
	}

	template <class OutputIt, class Function, class... Args>
	requires std::invocable<Function, Args...>
		  && std::output_iterator<OutputIt, std::invoke_result_t<Function, Args...>>
	[[nodiscard]] static auto time_function_repeatedly(
		std::size_t repeats, OutputIt outputIt, Function&& f, Args&&... args) -> Statistics<Duration>
	{
		std::vector<Duration> results;
		results.reserve(repeats);
		std::generate_n(std::back_inserter(results), repeats,
			// Here are various ways in which i tried perfect forwarding in a lambda:
			// works:
			[&outputIt, &f, &args...]
			// works:
			//[&outputIt, &f, ... args = std::forward<Args>(args)]
			// compiler error if I also forward f in the body:
			//[&outputIt, f = std::forward<Function>(f), &args...]
			// compiler error if I also forward f in the body:
			//[&outputIt, f = std::forward<Function>(f), ... args = std::forward<Args>(args)]
			{
				return Timer::time_function(
					*outputIt++, std::forward<Function>(f), std::forward<Args>(args)...);
			}
		);
		return Statistics<Duration>(std::move(results));
	}
};

