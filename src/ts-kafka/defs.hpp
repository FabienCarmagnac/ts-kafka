#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <map>
#include <functional>

namespace ts_kafka
{

	struct def_arg
	{
		std::string endpoint;
		std::string topic;
		std::map<std::string, std::string> params;
	};
	
	typedef std::chrono::system_clock::time_point tp;

	constexpr int64_t to_kafka_ts(tp t)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(t - tp()).count();
	}

	template < class T >
	struct timed_data
	{
		tp ts;// time of the object
		T object;
	};

	typedef timed_data<std::vector<char>> timed_data_raw;

	struct ts_interest
	{
		std::string endpoint;
		std::string topic;
		size_t object_size;
	};


	constexpr tp::duration duration_zero() { return std::chrono::milliseconds{ 0 }; }
}
