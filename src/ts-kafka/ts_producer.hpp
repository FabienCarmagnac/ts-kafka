#pragma once

#include "ts-kafka/defs.hpp"

#include "ts-kafka/include_rdkafka.h"

namespace ts_kafka
{
	enum producer_status
	{
		ok,
		busy, // the bus is occupied, try later. data not saved
		error,  // the bus is broken, drop and reconnect
	};

	class ts_producer	
	{

		class dr : public RdKafka::DeliveryReportCb
		{
		public:
			void dr_cb(RdKafka::Message &message);
		};

		dr _dr_cb;
		std::unique_ptr<RdKafka::Producer> _producer;

	public:

		ts_producer();
		virtual ~ts_producer();

		bool connect(const def_arg &, std::string & errstr);

		producer_status push(const std::string & topic, const timed_data_raw & data);


	private:

	};


}