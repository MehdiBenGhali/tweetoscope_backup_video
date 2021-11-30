#pragma once

#include "cascade.hpp"
#include "tweetoscopeCollectorParams.hpp"

#include <iostream>
#include <ostream>
#include <cppkafka/cppkafka.h>
#include <vector> 
#include <string>
#include <memory>

using namespace cppkafka ;

namespace tweetoscope {
class Cascade;
using wref_cascade = std::weak_ptr<Cascade> ;

struct serie_Producer : public cppkafka::Producer {
    private:
        std::string topic;

    public:
        serie_producer() = default; 
        serie_Producer(const serie_Producer&) = default;
        serie_producer& operator=(const serie_Producer&) = default;
        serie_Producer(std::string& topic_name, Configuration& config); //Constructor from builder and producer params
        serie_Producer(const tweetoscope::params::collector params); //Constructor from params collector object
        serie_Producer(const tweetoscope::params::collector params, std::string out_category); //Constructor from params collector object with output topic specification

        void produce(ref_cascade const& rcascade, double const& obs_time){}; //Produces a cascade as a tweet series
};

using times_vect = std::vector<double> ;

struct size_Producer : public cppkafka::Producer {
    private:
        std::string topic;

    public:
        size_producer() = default; 
        size_Producer(const serie_Producer&) = default;
        size_producer& operator=(const serie_Producer&) = default;
        size_Producer(std::string& topic_name, Configuration& config); //Constructor from builder and producer params
        size_Producer(const tweetoscope::params::collector params); //Constructor from params collector object
        size_Producer(const tweetoscope::params::collector params, std::string out_category); //Constructor from params collector object with output topic specification

        void produce(ref_cascade const& rcascade, times_vect const& times){}; //Produces a cascade size message for each predictor
};
}