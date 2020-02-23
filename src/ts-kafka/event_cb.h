#pragma once

#include <iostream>
#include "ts-kafka/include_rdkafka.h"


namespace ts_kafka
{
    class LogEventCb : public RdKafka::EventCb {
    public:
        void event_cb(RdKafka::Event& event)
        {
            switch (event.type())
            {
            case RdKafka::Event::EVENT_ERROR:
                std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
                    event.str() << std::endl;
                break;

            case RdKafka::Event::EVENT_STATS:
                std::cerr << "STATS: " << event.str() << std::endl;
                break;

            case RdKafka::Event::EVENT_LOG:
                std::cerr << "LOG-" << event.severity() << "-"
                    << event.fac() << ": " << event.str() << std::endl;
                break;

            default:
                std::cerr << "EVENT " << event.type() <<
                    " (" << RdKafka::err2str(event.err()) << "): " <<
                    event.str() << std::endl;
                break;
            }
        }
    };
    
    inline static LogEventCb& static_LogEventCb()
    {
        static LogEventCb l;
        return l;
    }
}


