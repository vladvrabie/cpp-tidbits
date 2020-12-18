#pragma once

#include <atomic>
#include <iostream>

template <class T>
struct SelfCountingType
{
	static std::atomic<std::size_t> instances;
	T value;

	SelfCountingType() : value{} { ++instances; };

	explicit SelfCountingType(const T& other_value) : value{ other_value } { ++instances; }

	SelfCountingType(const SelfCountingType& other) : value{ other.value } { std::cout << "CopC"; ++instances; }

	SelfCountingType(SelfCountingType&& other) noexcept : value{ std::move(other.value) } { std::cout << "MovC"; ++instances; }

	SelfCountingType& operator= (const SelfCountingType& other)
	{
		std::cout << "CopA";
		if (this != &other)
			value = other.value;
		return *this;
	}

	SelfCountingType& operator= (SelfCountingType&& other) noexcept
	{
		std::cout << "MovA";
		if (this != &other)
			value = std::move(other.value);
		return *this;
	}

	SelfCountingType& operator= (const T& other_value)
	{
		value = other_value;
		return *this;
	}

	~SelfCountingType() { --instances; }

	SelfCountingType& operator++()
	{
		++value;
		return *this;
	}

	friend bool operator== (const SelfCountingType& lhs, const SelfCountingType& rhs)
	{
		return lhs.value == rhs.value;
	}
};

template <class T>
std::atomic<std::size_t> SelfCountingType<T>::instances = 0;
