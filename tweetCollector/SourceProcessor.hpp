#pragma once

#include "cascade.hpp"
#include "producer.hpp"
#include "tweet.hpp"
#include "tweetoscopeCollectorTweetReader.cpp"
#include "tweetoscopeCollectorParams.hpp"

#include <memory>
#include <boost/heap/binomial_heap.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>

namespace tweetoscope {
class Cascade;
class serie_producer; 
class size_producer;

using wref_cascade = std::weak_ptr<Cascade>;

class Processor{
    private : 
        //Producers 
        tweetoscope::serie_producer serial;
        tweetoscope::size_producer sizal;
        //tweetoscope::log_producer logger """to implement"""

        //parameters
        double expiration_time;
        std::vector<double> times;
        unsigned int this_collection_source; 
        int min_cascade_size;

        //Collections 
        tweetoscope::priority_queue cascade_queue;
        std::map<std::string, wref_cascade> cascades_map;
        std::map<double, std::queue<wref_cascade>> partial_cascade_map;

    public : 
        Processor() = default; 
        Processor(const Tweet&) = default;
        Processor& operator=(const Processor&) = default; //Default constructors

        //Constructor from params
        Processor(tweetoscope::params::collector params,tweetoscope::serie_producer& serial,
                     tweetoscope::size_producer& sizal, int collection_source);


        ~Processor(){}; //Destructor

        //Updates queue when a tweet arrives
        void update(Tweet const& tweet){}; 
        //Extracts terminated cascades and publishes them
        void extract(Tweet const& tweet){}; 
};
}