#include "ts-kafka/basic_consumer.hpp"

#include <mutex>
#include <algorithm>
#include <condition_variable>

namespace ts_kafka
{
	void basic_consumer::rb_cb::rebalance_cb(RdKafka::KafkaConsumer *consumer, RdKafka::ErrorCode err, std::vector<RdKafka::TopicPartition*>&partitions)
	{

		if (err == RdKafka::ERR__ASSIGN_PARTITIONS)
		{
			for (auto & p : partitions)
				if (max_offset < p->offset())
					max_offset = p->offset();

			if (user_requested_offset < 0)
			{
				for (auto & p : partitions)
					p->set_offset(max_offset);

			}
			else
			{
				for (auto & p : partitions)
					p->set_offset(user_requested_offset);
			}

			consumer->assign(partitions);
		}
	}

	basic_consumer::rb_cb::~rb_cb() {}

	basic_consumer::basic_consumer()
	{
	}

	basic_consumer::~basic_consumer()
	{
		stop();
	}

#if 0

	/*bool basic_consumer::try_get_last(const def_arg & d, timed_data<std::vector<char>> & ret, std::string & errstr)
	{
		return try_get_at_offset(d, -1, ret, errstr);
	}
	*/

	/*bool basic_consumer::try_get_at_offset(const def_arg & d, int requested_offset, timed_data<std::vector<char>> & ret, std::string & errstr)
	{
		basic_consumer t;
		start_args sa;
		sa.start_offset = requested_offset;
		((def_arg&)sa) = d;

		std::mutex m;
		std::condition_variable cv;
		std::atomic_bool done = false;

		bool b = t.start(sa, [&](tp t, const char * buf, size_t len)
		{
			ret.ts = t;
			ret.object.clear();
			ret.object.assign(buf, buf + len);

			done = true;
			cv.notify_all();

		}, errstr);

		if (!b)
			return false;

		std::unique_lock<std::mutex> lk(m);
		while(!done)
			cv.wait(lk);

		return true;
	}*/
	/*int64_t basic_consumer::get_max_offset()const
	{
		return _rb_cb.max_offset;
	}*/

	/*bool basic_consumer::try_get_all(const def_arg & d, std::vector<timed_data<std::vector<char>>> & ret, std::string & errstr)
	{
		basic_consumer consom;
		start_args sa;
		sa.start_offset = 0;
		((def_arg&)sa) = d;

		std::mutex m;
		std::condition_variable cv;
		std::atomic_bool done=false;

		ret.clear();
		long long ix = 0;
		bool b = consom.start(sa, [&](tp t, const char * buf, size_t len)
		{
			if (ret.empty())
				ret.resize(consom.get_max_offset());

			if (ix >= consom.get_max_offset())
				return;

			ret[ix].object.assign(buf, buf+len);
			ret[ix].ts = t;

			++ix;
			if (ix >= consom.get_max_offset())
			{
				done = true;
				cv.notify_all();
			}

		}, errstr);

		if (!b)
			return false;

		std::unique_lock<std::mutex> lk(m);
		while(!done)
		{
			cv.wait(lk);
		}

		return true;
	}*/

#endif // 0


	void basic_consumer::wait()
	{
		std::unique_lock<std::mutex> l(_mx);
		while(!_stop)
			_cv.wait(l);
	}

	bool basic_consumer::pre_start(const consumer_start_args& sa, std::string & errstr)
	{
		std::unique_ptr<RdKafka::Conf> gconf(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));

		if (gconf->set("metadata.broker.list", sa.endpoint, errstr) != RdKafka::Conf::CONF_OK)
			return false;

		if (gconf->set("group.id", std::to_string(rand()), errstr) != RdKafka::Conf::CONF_OK)
			return false;

		if (gconf->set("rebalance_cb", &_rb_cb, errstr) != RdKafka::Conf::CONF_OK)
			return false;

		for (auto & kv : sa.params)
		{
			if (gconf->set(kv.first, kv.second, errstr) != RdKafka::Conf::CONF_OK)
				return false;
		}

		//_rb_cb.user_requested_offset = sa.start_offset;

		auto * p = RdKafka::Consumer::create(gconf.get(), errstr);
		if (!p)
			return false;
		_consumer.reset(p);

		auto * top = RdKafka::Topic::create(p, sa.topic, nullptr, errstr);
		if (!top)
			return false;
		_topic.reset(top);
		
		_queue.reset(RdKafka::Queue::create(p));

		return true;
	}

	void basic_consumer::stop()
	{
		if (_stop)
			return;

		_stop = true;
		
		_cv.notify_all();

		_consumer->stop(_topic.get(), 0);
		_th.join();
		_queue.reset();
		_topic.reset();
		_consumer.reset();

	}


	#if 0

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
#endif

}
