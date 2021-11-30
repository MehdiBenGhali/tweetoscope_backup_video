#pragma once

#include "Cascade.hpp"
#include "CollectorParams.hpp"

#include <iostream>
#include <ostream>
#include <cppkafka/cppkafka.h>
#include <vector> 
#include <string>
#include <memory>

using namespace cppkafka ;

namespace tweetoscope {

using wref_cascade = std::weak_ptr<tweetoscope::tweetCascade> ;
using ref_cascade = std::shared_ptr<tweetoscope::tweetCascade> ;


struct serie_Producer : public cppkafka::Producer {
    private:
        std::string topic;

    public:
        serie_Producer(const serie_Producer&) = default;
        serie_Producer& operator=(const serie_Producer&) = default;
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
        size_Producer(const size_Producer&) = default;
        size_Producer& operator=(const size_Producer&) = default;
        size_Producer(std::string& topic_name, Configuration& config); //Constructor from builder and producer params
        size_Producer(const tweetoscope::params::collector params); //Constructor from params collector object
        size_Producer(const tweetoscope::params::collector params, std::string out_category); //Constructor from params collector object with output topic specification

        void produce(ref_cascade const& rcascade, times_vect const& timewindows){}; //Produces a cascade size message for each predictor
};
}