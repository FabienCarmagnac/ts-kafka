# ts-kafka
timeseries c++ lib for kafka, as a [librdkafka](https://github.com/edenhill/librdkafka) wrapper.

# State of the project : DRAFT !!! DO NOT USE IT YET !!!

### Use case

A consumer process using time-indexed data (timeseries) sometimes need to read a stream of temporal data from some past date till now, and then continue to receive the new data feeded by some real-time producer. With ts-kakfa, the transition from historical to real time is transparent.

### Where come real-life problems ...

#### Init problem

ts-kafka handles the storage problem of the historical and real-time data during initialisation.
Let's consider the use case where you come with some historical data and wants to start recording real-time-labeled data. First, the sequence is empty. So, you should insert the historical data in kafka while recording the real-time data some where else. The reconciliation of the time sequence must wait the historical insertion timestamp reaches real-time first timestamp.

ts-kafka handles this for you.

#### Recent past for consumer

From a consumer point of view, subscribing to timed-labeled data usually comes with the need of recent past.
ts-kafka consumer can provide historical data the same way than its notifies new data updates. This historical data can be request by past dates or number of elements.

### Platform

  * win x64
  * linux x64

### DONE

  * first draft of headers : consumer and producer
  * simple test compiling : 1 real-time producer writes the timestamp pushed and the index of the message. The consumer verifies the notification time of the enngine is the same than the timestamp inside the payload message and its index.
  * build a kafka sandbox

## Draft spec

### Data

The data is represented by a binary block (as std::vector<char>) + a utc timestamp.

### basic_consumer

This class consumes data from a topic, between 2 dates.
A blocking `stop()`  method halts the notification in a async way.
The seek of the start target date is done by dichotomy.

### basic_producer

This class stores the data without timestamp ordering control. It just stores.
Can store in async or in sync way.

### ts_producer_rt

This class stores the real-time (rt) data.
Storage calls can be sync or async.
This class knows the timestamp of last record.

On start, we assume a ts_producer_rt has a very recent block of historical data available.
If the last date of topic T < first date of recent block, the producer will write the data in a T-head topic, not T. This way it avoids 'holes' in the T main topic.
This producer uses a basic_consumer to detect the changes on topic T. 
When the last date of topic T >= first date of recent block, the producer will transfert the data from T-head to T and then, do not write anymore to T-head but directly to T.

Only one instance of ts_producer_rt should be runned on a topic at a time.

### ts_producer_histo

This class is used to load in kafka the first snapshot, typically offline.
This class knows the timestamp of last record.
Only one instance of ts_producer_histo should be runned on a topic at a time.



### Optimizarion to be done 

 - allocator for message 
 
  
