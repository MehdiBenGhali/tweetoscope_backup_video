#pragma once

#include "Cascade.hpp"
#include "tweetoscopeProducer.hpp"
#include "tweet.hpp"
#include "CollectorParams.hpp"

#include <memory>
#include <boost/heap/binomial_heap.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>

namespace tweetoscope {

class serie_Producer;
class size_Producer;

class Processor{
    private : 
        //Producers 
        tweetoscope::serie_Producer& serial;
        tweetoscope::size_Producer& sizal;
        //tweetoscope::log_producer logger """to implement"""

        //parameters
        double expiration_time;
        std::vector<double> timewindows;
        unsigned int this_collection_source; 
        double min_cascade_size;

        //Collections 
        tweetoscope::priority_queue cascade_queue;
        std::map<std::string, std::weak_ptr<tweetoscope::tweetCascade>> cascades_map;
        std::map<double, std::queue<std::weak_ptr<tweetoscope::tweetCascade>>> partial_cascade_map;

    public : 
        Processor& operator=(const Processor&) = default; //Default constructors

        //Constructor from params
        Processor(tweetoscope::serie_Producer& serial,tweetoscope::size_Producer& sizal,
                    tweetoscope::params::collector params, int collection_source);

        ~Processor(){}; //Destructor

        //Run processor treatments upon tweet arrival
        void process(tweetoscope::Tweet const& tweet, std::string const& cascade_id); 

        //Extracts terminated cascades and publishes them
        void extractExpired(tweetoscope::Tweet const& tweet); 

        //Post partial active cascades on time of tweet
        void postPartials(tweetoscope::Tweet const& tweet);

        //create new cascade or update existing one 
        void updateCascades(tweetoscope::Tweet const& tweet, std::string const& cascade_id);
};
}