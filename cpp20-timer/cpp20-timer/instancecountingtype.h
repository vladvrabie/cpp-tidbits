#pragma once

#include <atomic>
#include <iostream>

template <class T>
struct InstanceCountingType
{
	static std::atomic<std::size_t> instances;
	static bool logging;
	T value;

	InstanceCountingType()
		: value{}
	{
		++instances;
	};

	explicit InstanceCountingType(const T& other_value)
		: value{ other_value }
	{
		++instances;
	}

	InstanceCountingType(const InstanceCountingType& other)
		: value{ other.value }
	{
		if (logging)
			std::cout << "CopC";
		++instances;
	}

	InstanceCountingType(InstanceCountingType&& other) noexcept
		: value{ std::move(other.value) }
	{
		if (logging)
			std::cout << "MovC";
		++instances;
	}

	InstanceCountingType& operator= (const InstanceCountingType& other)
	{
		if (logging)
			std::cout << "CopA";
		if (this != &other)
			value = other.value;
		return *this;
	}

	InstanceCountingType& operator= (InstanceCountingType&& other) noexcept
	{
		if (logging)
			std::cout << "MovA";
		if (this != &other)
			value = std::move(other.value);
		return *this;
	}

	InstanceCountingType& operator= (const T& other_value)
	{
		value = other_value;
		return *this;
	}

	~InstanceCountingType()
	{
		--instances;
	}

	InstanceCountingType& operator++()
	{
		++value;
		return *this;
	}

	friend bool operator== (const InstanceCountingType& lhs, const InstanceCountingType& rhs)
	{
		return lhs.value == rhs.value;
	}
};

template <class T>
std::atomic<std::size_t> InstanceCountingType<T>::instances = 0;

template <class T>
bool InstanceCountingType<T>::logging = false;
