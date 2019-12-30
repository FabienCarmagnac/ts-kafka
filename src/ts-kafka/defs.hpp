#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <functional>

namespace ts_kafka
{

	typedef std::chrono::system_clock::time_point tp;

	template < class T >
	struct timed_data
	{
		tp ts;// time of the object
		T* object;
	};

	struct ts_interest
	{
		std::string endpoint;
		std::string topic;
		size_t object_size;
	};


	constexpr tp::duration duration_zero() { return std::chrono::milliseconds{ 0 }; }
}
