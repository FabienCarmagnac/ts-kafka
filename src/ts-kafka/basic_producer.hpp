#pragma once

#include <memory>
#include <atomic>
#include <thread>

#include "ts-kafka/defs.hpp"
#include "ts-kafka/include_rdkafka.h"

namespace ts_kafka
{
	enum class producer_status
	{
		ok,
		busy, // the bus is occupied, try later. data not saved
		error,  // the bus is broken, drop and reconnect
	};

	class basic_producer
	{

		class dr : public RdKafka::DeliveryReportCb
		{
		public:
			void dr_cb(RdKafka::Message& message);
		};


		class ev : public RdKafka::EventCb
		{
		public:
			void event_cb(RdKafka::Event& event);
		};

		std::string _topic;
		dr _dr_cb;
		ev _ev;
		std::unique_ptr<RdKafka::Producer> _producer;
		std::thread _poll_th;
		std::atomic_bool _stop;

		void bg_pool();

	public:

		basic_producer();
		virtual ~basic_producer();

		bool connect(const def_arg &, std::string & errstr);

		producer_status push(const timed_data_raw & data, std::atomic<bool>* cancel_token = nullptr);


	private:

	};


}