#pragma once

namespace ts_kafka
{

	enum producer_status
	{
		ok,
		busy, // the bus is occupied, try later. data not saved
		connect_error,  // the bus is broken, drop and reconnect
	};

}