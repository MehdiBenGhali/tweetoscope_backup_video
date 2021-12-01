#pragma once
#include <iostream>
#include <ostream>
#include "cppkafka/cppkafka.h"
#include "CollectorParams.hpp"

namespace tweetoscope {
class tweetConsumer : public cppkafka::Consumer {
    private:
        std::string topic;
        bool subscribed;

    public:
        // Class constructors
        tweetConsumer(cppkafka::Configuration& config, std::string topic);
        tweetConsumer(tweetoscope::params::collector params);
        
        // Methods
        bool isSubscribed() {return subscribed;}
        std::string getTopic() {return topic;}
        void subscribeTopic();
    };
}

