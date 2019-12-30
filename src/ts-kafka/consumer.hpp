#pragma once

#include <memory>
#include "defs.hpp"
#include "circular_buffer.hpp"

namespace ts_kafka
{

	class consumer
	{
		consumer();
	public:

		virtual ~consumer();

		/* any failure returns {}. No exceptions */
		static std::unique_ptr<consumer> try_create(const ts_interest &, std::string & err_msg);

		struct start_args
		{
			tp historical_startdate_storage = tp::max();
			tp::duration keep_time_window = duration_zero();
			size_t keep_size_window = 1; // keep just last one
		};

		struct data_notification
		{
			circular_buffer::iterator begin;
			circular_buffer::iterator end;
		};

		/* multiple call erase the previous one. when callback is invoke with null ptr, end of topic */
		void start(std::function<void(const data_notification &)> callback, const start_args &);

		void stop();

	};
}