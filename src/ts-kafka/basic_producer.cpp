#include "ts-kafka/basic_producer.hpp"
#include "ts-kafka/event_cb.h"

#include <iostream>

namespace ts_kafka
{


	basic_producer::basic_producer()
	{}
	basic_producer::~basic_producer()
	{
		_stop = true;
		_poll_th.join();
	}
	producer_status basic_producer::push(const timed_data_raw& data, std::atomic<bool> * cancel_token)
	{
		while (1)
		{
			auto err = _producer->produce(
				_topic,
				RdKafka::Topic::PARTITION_UA,
				RdKafka::Producer::RK_MSG_COPY,
				(void*)data.object.data(), data.object.size(),
				nullptr, 0,
				to_kafka_ts(data.ts),
				nullptr);

			switch (err)
			{
			case RdKafka::ERR_NO_ERROR:
				_producer->poll(1);
				return producer_status::ok;

			case RdKafka::ERR__QUEUE_FULL:
				if (cancel_token && *cancel_token)
					return producer_status::busy;
				_producer->poll(1);
				continue;

			default:
				return producer_status::error;
			};
		}

	}

	void basic_producer::dr::dr_cb(RdKafka::Message& m)
	{
		if (m.err() != RdKafka::ERR_NO_ERROR)
		{
			std::cout << m.errstr() << "\n";
		}
	}
	void basic_producer::bg_pool()
	{
		while (!_stop)
			_producer->poll(500);
	}

	void basic_producer::ev::event_cb(RdKafka::Event& event)
	{
		std::cout << event.str() << "\n";
	}
	bool basic_producer::connect(const def_arg & sa, std::string & errstr)
	{
		std::unique_ptr<RdKafka::Conf> conf(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));

		if (conf->set("security.protocol", "sasl_plaintext", errstr) != RdKafka::Conf::CONF_OK)
			return false;


		if (conf->set("debug", "all", errstr) != RdKafka::Conf::CONF_OK)
			return false;
		
		if (conf->set("metadata.broker.list", sa.endpoint, errstr) != RdKafka::Conf::CONF_OK)
			return false;

		if (conf->set("event_cb", &static_LogEventCb(), errstr) != RdKafka::Conf::CONF_OK)
			return false;

		if (conf->set("message.timeout.ms", "1000", errstr) != RdKafka::Conf::CONF_OK)
			return false;

		if (conf->set("sasl.mechanism", "PLAIN", errstr) != RdKafka::Conf::CONF_OK)
			return false;

		if (conf->set("sasl.username", "dev1", errstr) != RdKafka::Conf::CONF_OK)
			return false;

		if (conf->set("sasl.password", "dev1_", errstr) != RdKafka::Conf::CONF_OK)
			return false;

		if (conf->set("dr_cb", &_dr_cb, errstr) != RdKafka::Conf::CONF_OK)
			return false;

		for (auto & kv : sa.params)
		{
			if (conf->set(kv.first, kv.second, errstr) != RdKafka::Conf::CONF_OK)
				return false;
		}

		auto * p = RdKafka::Producer::create(conf.get(), errstr);
		if (!p)
			return false;

		_producer.reset(p);

		_poll_th = std::thread(&basic_producer::bg_pool, this);

		return true;
	}



}