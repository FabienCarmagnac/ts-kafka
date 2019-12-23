# ts-kafka
timeseries c++ lib for kafka, based on [librdkafka](https://github.com/edenhill/librdkafka)

### Use case

A process using time-indexed data (timeseries) sometimes need to read a stream of temporal data from some past date till now, and then continue to receive the new data feeded by some real-time producer.

The goal of this lib is to reconciliate the 2 behaviours (consumer and producer).

### Implem 

Each X topic has an index topic named X-index which stores `<timestamps, offset>` of the partition X.
The producer stores regulary in X-index topic a `<timestamps, offset>` element.
The consumer uses this X-index topic to retrieve the offset from which the notification should start.

### Platform

  * win x64
  * linux x64
  
  
  
