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

	enum producer_status
	{
		ok,
		busy, // the bus is occupied, try later. data not saved
		connect_error,  // the bus is broken, drop and reconnect
	};

	class rt_producer
	{
	public:

		virtual ~rt_producer();

		/* any failure returns {}. No exceptions */
		static std::unique_ptr<rt_producer> try_create(const std::string & endpoint, const std::string & topic);

		/* get the last element produced OR the last element of the queue if the producer is idle.
		buffer must be large enough. if not, 'n' is impacted. */
		void fetch_last(timed_data & data);		

		producer_status push(const timed_data & data);
	};

	template < class T >
	class circular_buffer
	{
		class iterator;


	};
	class histo_producer
	{
	public:

		virtual ~histo_producer();

		/* any failure returns {}. No exceptions */
		static std::unique_ptr<histo_producer> try_create(const std::string & endpoint, const std::string & topic);

		/* get the last element produced OR the last element of the queue if the producer is idle.
		buffer must be large enough. if not, 'n' is impacted. */
		void fetch_last(timed_data & data);


		producer_status push(const timed_data & data);
	};


	/*
	consumer class is used to receives the data for a given topic.
	can be :
		- restarted several times	
		- stopped
	*/

	tp::duration duration_zero() { return std::chrono::milliseconds{ 0 }; }

	template < class T >
	class consumer
	{
		consumer();
	public:

		virtual ~consumer();

		/* any failure returns {}. No exceptions */
		static std::unique_ptr<consumer<T>> try_create(const std::string & endpoint, const std::string & topic);

		struct start_args
		{
			tp historical_startdate_storage = tp::max();
			tp::duration keep_time_window = duration_zero();
			size_t keep_size_window = 1; // keep just last one
		};

		/* multiple call erase the previous one. when callback is invoke with null ptr, end of topic */		
		void start(std::function<void(const timed_data &)> callback, const start_args & );

		void stop();

	};
}

