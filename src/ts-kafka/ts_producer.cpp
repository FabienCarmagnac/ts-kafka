#include "ts-kafka/ts_producer.hpp"

namespace ts_kafka
{


	ts_producer::ts_producer()
	{}
	ts_producer::~ts_producer()
	{}
	producer_status ts_producer::push(const std::string & topic, const timed_data_raw & data)
	{
		auto err = _producer->produce(
			topic,
			RdKafka::Topic::PARTITION_UA,
			RdKafka::Producer::RK_MSG_COPY,
			(void*)data.object.data(), data.object.size(),
			nullptr, 0,
			to_kafka_ts(data.ts),
			nullptr);

		return err == RdKafka::ERR_NO_ERROR
			? producer_status::ok
			: err == RdKafka::ERR__QUEUE_FULL
			? producer_status::busy
			: producer_status::error;

	}

	bool ts_producer::connect(const def_arg & sa, std::string & errstr)
	{

		std::unique_ptr<RdKafka::Conf> conf(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));

		if (conf->set("metadata.broker.list", sa.endpoint, errstr) != RdKafka::Conf::CONF_OK)
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
		return true;
	}



}