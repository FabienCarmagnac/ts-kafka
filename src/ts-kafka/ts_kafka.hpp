#pragma once

#include "ts-kafka/include_rdkafka.h"

namespace ts_kafka
{

	class rt_producer
	{
	public:

		virtual ~rt_producer();

		/* any failure returns {}. No exceptions */
		static std::unique_ptr<rt_producer> try_create(const ts_interest &);

		/* get the last element produced OR the last element of the queue if the producer is idle.
		buffer must be large enough. if not, 'n' is impacted. */
		void fetch_last(timed_data<T> & data);

		producer_status push(const timed_data<T> & data);
	};


	template < class serializer_t >
	class histo_producer
	{
	public:

		virtual ~histo_producer();

		/* any failure returns {}. No exceptions */
		static std::unique_ptr<histo_producer> try_create(const ts_interest & );

		/* get the last element produced OR the last element of the queue if the producer is idle.
		buffer must be large enough. if not, 'n' is impacted. */
		void fetch_last(timed_data<T> & data);


		producer_status push(const timed_data<T> & data);
	};


	/*
	consumer class is used to receives the data for a given topic.
	can be :
		- restarted several times	
		- stopped
	*/


}

