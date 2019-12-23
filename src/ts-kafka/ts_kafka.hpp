#pragma once

#include <chrono>
#include <string>
#include <functional>
#include <librdkafka/rdkafkacpp.h>

typedef std::chrono::system_clock::time_point tp;

namespace tskafka
{
	class producer
	{
	public:

		virtual ~producer();

		/* any failure returns {}. No exceptions */
		static std::unique_ptr<producer> try_create(const std::string & endpoint, const std::string & topic);

		/* get the last element produced OR the last element of the queue if the producer is idle.
		buffer must be large enough. if not, 'n' is impacted. */
		void fetch_last(const char * buffer, size_t & n);
		
		enum status 
		{
			ok,
			busy, // the bus is occupied, try later. data not saved
			connect_error,  // the bus is broken, drop and reconnect
		};

		status push(tp ts, const char * data, size_t size);
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

		/* multiple call erase the previous one */		
		void start(std::function<void(const char *, size_t)> callback, tp startdate, tp enddate = tp::max());
		void stop();

	};
}
