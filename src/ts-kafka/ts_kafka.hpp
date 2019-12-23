#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <functional>
#include <librdkafka/rdkafkacpp.h>


namespace ts_kafka
{
	typedef std::chrono::system_clock::time_point tp;

	struct timed_data
	{
		tp ts;// time of the object
		std::vector<char> buffer;

	};

	class producer
	{
	public:

		virtual ~producer();

		/* any failure returns {}. No exceptions */
		static std::unique_ptr<producer> try_create(const std::string & endpoint, const std::string & topic);

		/* get the last element produced OR the last element of the queue if the producer is idle.
		buffer must be large enough. if not, 'n' is impacted. */
		void fetch_last(timed_data & data);
		
		enum status 
		{
			ok,
			busy, // the bus is occupied, try later. data not saved
			connect_error,  // the bus is broken, drop and reconnect
		};

		status push(const timed_data & data);
	};


	/*
	consumer class is used to receives the data for a given topic.
	can be :
		- restarted several times	
		- stopped
	*/

	class consumer
	{
		consumer();
	public:

		virtual ~consumer();

		/* any failure returns {}. No exceptions */
		static std::unique_ptr<consumer> try_create(const std::string & endpoint, const std::string & topic);

		/* multiple call erase the previous one. when callback is invoke with null ptr, end of topic */		
		void start(std::function<void(const timed_data &)> callback, tp startdate, tp enddate = tp::max());
		void stop();

	};
}

