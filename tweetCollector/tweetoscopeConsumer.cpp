#include "tweetoscopeConsumer.hpp"

tweetoscope::tweetConsumer::tweetConsumer(cppkafka::Configuration& config, std::string topic)
	: cppkafka::Consumer(config), topic(topic), subscribed(false) {}	


tweetoscope::tweetConsumer::tweetConsumer(tweetoscope::params::collector params)
	: cppkafka::Consumer(
						 cppkafka::Configuration({
							 					  {"bootstrap.servers", params.kafka.brokers},
												  {"auto.offset.reset", params.consumerConfig.offset_reset},
												  { "group.id", params.consumerConfig.group_id }
						 						 })
						), 
	  topic(params.topic.in), subscribed(false) {}

void tweetoscope::tweetConsumer::subscribeTopic() {
	this->subscribe({topic});
    this->subscribed = true;
}


