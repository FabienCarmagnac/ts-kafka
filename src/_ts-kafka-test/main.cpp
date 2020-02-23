
#include "ts-kafka/ts_kafka.hpp"
#include <thread>
#include <atomic>
#include <iostream>

#pragma comment(lib,"ts-kafka.lib")

using namespace ts_kafka;
using namespace std;

int report_error(const string & s, int x)
{
	cout << "test failed with message: " << s << ", code " << x << endl;
	exit(x);
}

struct mydata_t
{
	int64_t time;
	uint64_t counter;
};

int main()
{
	const tp start_time_point = tp::clock::now();
	const string endpoint = "carmagnac-labs.org:9091"
		, topic= "test-topic";

	const tp now = tp::clock::now();
	const tp start = now + chrono::seconds(5);
	const tp end = start + chrono::seconds(5);

	thread produce_th([=] 
	{
		string errstr;
		basic_producer p0;
		def_arg da;
		//da.params
		da.endpoint = endpoint;
		da.topic = topic;
		
		if (!p0.connect(da, errstr))
			report_error("cant create producer because "+errstr, 10);

		timed_data_raw td;
		td.object.resize(sizeof(mydata_t));
		// create a mydata_t obj in timed_data object
		mydata_t * mydata = new (&td.object[0]) mydata_t;
		mydata->counter = 0;

		while (tp::clock::now() < end + chrono::seconds(2))
		{
			auto n = tp::clock::now();
			td.ts = n;
			mydata->time = chrono::duration_cast<chrono::milliseconds>(n.time_since_epoch()).count();

			//cout << "+";
			auto e = p0.push(td);

			if (e == producer_status::error) //ko : bye
			{
				report_error("connect/internal error, exiting", 11);
			}

			// ok
			mydata->counter++;
			this_thread::sleep_for(chrono::milliseconds(1000)); // sleep a bit to avoid massive amount in the cluster

		}
	});

	
	string errstr;
	basic_consumer c0;
	consumer_start_args sa;
	sa.endpoint = endpoint;
	sa.start_offset = 0;
	sa.topic = topic;

	atomic_uint64_t counter = 0;
		
	bool ok = c0.start(sa, [&](tp ts, const char* buf, size_t siz)
	{

		cout << ".";
		mydata_t * t = (mydata_t*)buf;

		// did we miss messages ? check counter 
		if (counter == 0 || t->counter == counter + 1)
			counter = t->counter;
		else
		{
			report_error("missed data", 13);
		}

		if (ts != chrono::milliseconds(t->time) + tp())
		{
			report_error("times are not sync", 14);
		}

		if (ts < start)
		{
			report_error("data before start", 15);
		}
		if (end < ts)
		{
			report_error("data after end ", 16);
		}


	}, errstr);

	if (!ok)
		report_error("cant create consumer: " + errstr, 12);

	std::cout << "waiting 10s\n";
	std::this_thread::sleep_for(std::chrono::seconds(10));

	c0.stop();

	//produce_th.join();
	   
	cout << "=== end ===" << endl;
	return 0;
}