#pragma once

#include <memory>
#include "ts-kafka/defs.hpp"

#include "ts-kafka/include_rdkafka.h"

#include <thread>
#include <map>
#include <atomic>

namespace ts_kafka
{

	// single/basic consumer, no logic
	class ts_consumer
	{
	public:


		struct start_args : def_arg
		{
			int64_t start_offset;
			int consume_block_time_ms = 1000;
		};

		ts_consumer();
		virtual ~ts_consumer();

		// callback_t should be invokable as (tp, const char*, size_t)
		template < class callback_t >
		bool start(const start_args &, callback_t callback
			, std::string & errstr);

		void stop();

		static bool try_get_at_offset(const def_arg & d, int requested_offset, timed_data<std::vector<char>> & ret, std::string & errstr);
		static bool try_get_last(const def_arg & d, timed_data<std::vector<char>> & ret, std::string & errstr);
		static bool try_get_all(const def_arg & d, std::vector<timed_data<std::vector<char>>> & ret, std::string & errstr);

		// value fixed while rebalancing. 
		int64_t get_max_offset()const;

	private:

		class rb_cb : public RdKafka::RebalanceCb
		{
		public:
			std::atomic_int64_t user_requested_offset;
			std::atomic_int64_t max_offset=0;
			virtual void rebalance_cb(RdKafka::KafkaConsumer *consumer, RdKafka::ErrorCode err, std::vector<RdKafka::TopicPartition*>&partitions);
			virtual ~rb_cb();
		};

		std::atomic_bool _stop = false;
		std::thread _th;
		rb_cb _rb_cb;
		std::unique_ptr<RdKafka::KafkaConsumer> _consumer;

		template < class callback_t >
		void do_read(int bk_ms, callback_t i);

		bool pre_start(const start_args &, std::string & errstr);
	};

	template < class callback_t >
	bool ts_consumer::start(const start_args & sa, callback_t inserter
		, std::string & errstr)
	{
		if (!pre_start(sa, errstr))
			return false;

		_th = std::thread(&ts_consumer::do_read<callback_t>, this, sa.consume_block_time_ms, inserter);
		return true;
	}


	template < class callback_t >
	void ts_consumer::do_read(int bk_ms, callback_t cb)
	{
		while (!_stop)
		{
			RdKafka::Message *msg = _consumer->consume(bk_ms);
			if (! msg || !msg->len())
			{
				//cout << " null ";
				continue;
			}

			//cout << "inc:";
			cb(tp() + std::chrono::milliseconds(msg->timestamp().timestamp), (const char*) msg->payload(), msg->len());

			//msg->timestamp
			delete msg;
		}
	}
#if 0

		/* any failure returns {}. No exceptions */
		static std::unique_ptr<ts_consumer> try_create(const ts_interest &, std::string & err_msg);

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
		void start(std::function<void(const data_notification &)> callback, const start_args &);


	};

#endif

}