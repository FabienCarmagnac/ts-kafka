# ts-kafka
timeseries c++ lib for kafka, as a [librdkafka](https://github.com/edenhill/librdkafka) wrapper.

# State of the project : PRE-ALPHA

### Use case

A consumer process using time-indexed data (timeseries) sometimes need to read a stream of temporal data from some past date till now, and then continue to receive the new data feeded by some real-time producer. With ts-kakfa, the transition from historical to real time is transparent.

### Where comes real-life problem(s) ...

#### Init problem

ts-kafka handles the storage problem of the historical and real-time data during initialisation.
Let's consider the use case where you come with some historical data and wants to start recording real-time-labeled data. First, the sequence is empty. So, you should insert the historical data in kafka while recording the real-time data some where else. The reconciliation of the time sequence must wait the historical insertion timestamp reaches real-time first timestamp.

ts-kafka handles this for you.

#### Recent past for consumer

From a consumer point of view, subscribing to timed-labeled data usually comes with the need of recent past.




At the beginning you start from an empty time-ordered sequence.
Now, you want to initialize it with some historical data.
But you want, now, also, to start recording some real-time data.

And you want, now, also, to start a consumer, which has the ability to subscribe to topic with recording some real-time data.

ts-kafka distinguishes 2 kind of producers : historical and real-world.
The historical producer just write the data typically reading from other storage but will never be as fresh as now.
The real-world producer, typically real time, collects data. 


### Implem 

#### Producers

Basic producer class is used to implement the real ts-kafka producer

#### Consumr

Each X topic has an index topic named X-index which stores `<timestamps, offset>` of the partition X.
The producer stores regulary in X-index topic a `<timestamps, offset>` element.
The consumer uses this X-index topic to retrieve the offset from which the notification should start.

### Platform

  * win x64
  * linux x64

### DONE

  * first draft of headers : consumer and producer
  * simple test compiling : one producer writes the timestamp pushed and the index of the message. The consumer verifies the notification time of the enngine is the same than the timestamp inside the payload message and its index.
  
### TODO

  * build a kafka sandbox
  
### How it works ?

## Producer
  
### Remark

This will be a fu**** awesome lib.
