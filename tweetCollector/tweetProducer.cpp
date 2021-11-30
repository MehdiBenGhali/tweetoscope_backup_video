#pragma once

#include "tweetProducer.hpp"

#include <cppkafka/cppkafka.h>
#include <vector> 
#include <string>
#include <memory>

//Constructor from builder and producer params
tweetoscope::serie_producer::serie_Producer(std::string& topic_name, Configuration& config)
    : cppkafka::Producer(config), topic(topic) {}

//Constructor from params collector
tweetoscope::serie_producer::serie_Producer(const tweetoscope::params::collector params)
    : cppkafka::Producer({{"bootstrap.servers", params.kafka.brokers}}), topic(params.topic.in) {}

//Constructor from params collector object with output topic specification
tweetoscope::serie_producer::serie_Producer(const tweetoscope::params::collector params, std::string out_category)
	: cppkafka::Producer({{"bootstrap.servers", params.kafka.brokers}}) {
		if (out_category == "out_properties")
			topic = params.topic.out_properties;
		else if (out_category == "out_series")
			topic = params.topic.out_series;
	}

//Produces a cascade as a tweet series
void tweetoscope::serie_producer::produce(ref_cascade const& rcascade,double const& obs_time){
    this->produce(MessageBuilder(topic).payload(rcascade->toSeries(obs_time)));
}

//Constructor from builder and producer params
tweetoscope::size_producer::size_Producer(std::string& topic_name, Configuration& config){}
    : cppkafka::Producer(config), topic(topic) {}

//Constructor from params collector object with output topic specification
tweetoscope::size_producer::size_Producer(const tweetoscope::params::collector params, std::string out_category)
	: cppkafka::Producer({{"bootstrap.servers", params.kafka.brokers}}) {
		if (out_category == "out_properties")
			topic = params.topic.out_properties;
		else if (out_category == "out_series")
			topic = params.topic.out_series;
	}

//Produces a cascade size message for each predictor
void tweetoscope::size_producer::produce(ref_cascade const& rcascade, times_vect const& timewindows){
    for (const auto& obs_time : timewindows) {
        this->produce(MessageBuilder(topic).key(obs_time).payload(rcascade->toSize()));
    }
}