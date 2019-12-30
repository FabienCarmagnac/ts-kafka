
#include "ts-kafka/ts_kafka.hpp"
#include <thread>
#include <atomic>
#include <iostream>

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
	tp start_time_point = tp::clock::now();
	string endpoint = "carmagnac-labs.org:9091"
		, topic= "test-topic";

	tp now = tp::clock::now();
	tp start = now + chrono::seconds(5);
	tp end = start + chrono::seconds(5);

	thread produce_th([=] 
	{
		auto p0 = rt_producer::try_create(endpoint, topic);
		if(!p0)
			report_error("cant create producer", 10);

		timed_data td;
		td.buffer.resize(sizeof(mydata_t));
		// create a mydata_t obj in timed_data object
		mydata_t * mydata = new (&td.buffer[0]) mydata_t;
		mydata->counter = 0;

		while (tp::clock::now() < end + chrono::seconds(2))
		{
			td.ts = tp::clock::now();
			mydata->time = chrono::duration_cast<chrono::milliseconds>(td.ts - start_time_point).count();

			int e = p0->push(td);

			if (e == producer_status::busy) // busy, retry in a few ms
			{
				this_thread::sleep_for(chrono::milliseconds(100));
				continue;
			}

			if (e == producer_status::connect_error) //ko : bye
			{
				report_error("connect/internal error, exiting", 11);
			}

			// ok
			mydata->counter++;
			this_thread::sleep_for(chrono::milliseconds(0)); // sleep a bit to avoid massive amount in the cluster

		}
	});

	thread cons_th([=]
	{
		auto c0 = consumer::try_create(endpoint, topic);
		if(!c0)
			report_error("cant create consumer", 12);

		atomic_uint64_t counter = 0;
		c0->start([&](const timed_data & td)
		{

			mydata_t * t = (mydata_t*)&td.buffer[0];

			// did we miss messages ? check counter 
			if (counter == 0 || t->counter == counter + 1)
				counter = t->counter;
			else
			{
				report_error("missed data", 13);
			}

			if (td.ts != chrono::milliseconds(t->time) + start_time_point)
			{
				report_error("times are not sync", 14);
			}

			if (td.ts < start)
			{
				report_error("data before start", 15);
			}
			if (end < td.ts)
			{
				report_error("data after end ", 16);
			}


		}, start, end);

	});

	produce_th.join();
	cons_th.join();
	   
	cout << "=== end ===" << endl;
	return 0;
}