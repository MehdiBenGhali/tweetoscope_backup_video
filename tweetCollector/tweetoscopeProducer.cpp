#include "tweetoscopeProducer.hpp"

#include <cppkafka/cppkafka.h>
#include <vector> 
#include <string>
#include <memory>

//Constructor from builder and producer params
tweetoscope::serie_Producer::serie_Producer(std::string& topic_name, cppkafka::Configuration& config)
    : cppkafka::Producer(config), topic(topic_name) {}

//Constructor from params collector
tweetoscope::serie_Producer::serie_Producer(const tweetoscope::params::collector& params)
    : cppkafka::Producer({{"bootstrap.servers", params.kafka.brokers}}), topic(params.topic.in) {}

//Constructor from params collector object with output topic specification
tweetoscope::serie_Producer::serie_Producer(const tweetoscope::params::collector& params, std::string& out_category)
	: cppkafka::Producer({{"bootstrap.servers", params.kafka.brokers}}) {
		if (out_category == "out_properties")
			topic = params.topic.out_properties;
		else if (out_category == "out_series")
			topic = params.topic.out_series;
	}

//Produces a cascade as a tweet series
void tweetoscope::serie_Producer::post(tweetoscope::ref_cascade const& rcascade,double const& obs_time){
	std::string payload =rcascade->toSeries(obs_time);
    this->produce(MessageBuilder(topic).payload(payload));
	this->flush();
}

//Constructor from builder and producer params
tweetoscope::size_Producer::size_Producer(std::string& topic_name,  cppkafka::Configuration& config)
    : cppkafka::Producer(config), topic(topic_name) {}

//Constructor from params collector object
tweetoscope::size_Producer::size_Producer(const tweetoscope::params::collector& params)
    : cppkafka::Producer({{"bootstrap.servers", params.kafka.brokers}}), topic(params.topic.in) {}

//Constructor from params collector object with output topic specification
tweetoscope::size_Producer::size_Producer(const tweetoscope::params::collector& params, std::string& out_category)
	: cppkafka::Producer({{"bootstrap.servers", params.kafka.brokers}}) {
		if (out_category == "out_properties")
			topic = params.topic.out_properties;
		else if (out_category == "out_series")
			topic = params.topic.out_series;
	}

//Produces a cascade size message for each predictor
void tweetoscope::size_Producer::post(tweetoscope::ref_cascade const& rcascade, tweetoscope::times_vect const& timewindows){
    for (const auto& obs_time : timewindows) {
		std::string cle = std::to_string(obs_time);
		std::string payload = rcascade->toSize();
        this->produce(MessageBuilder(topic).key(cle).payload(payload));
		this->flush();
    }
}