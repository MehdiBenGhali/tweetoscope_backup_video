#pragma once

#include "cascade.hpp"
#include "kafka.hpp"
#include "tweet.hpp"

#include <memory>
#include <boost/heap/binomial_heap.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>



class ref_cascade_comparator {
    inline bool operator()(ref_cascade op1, ref_cascade op2) const {return *op1 < *op2;} 
}

using ref_cascade = std::shared_ptr<Cascade>;
using wref_cascade = std::weak_ptr<Cascade>;
using priority_queue = boost::heap::binomial_heap<ref_cascade, boost::heap::compare<ref_cascade_comparator>>;

class Processor{
    private : 
        priority_queue queue;
    
    public : 
        Processor() = default; 
        Processor(const Tweet&) = default;
        Processor& operator=(const Processor&) = default; //Default constructors
        Processor(Cascade const& cascade){}; //Constructor from a cascade
        Processor(Tweet const& tweet){}; //Constructor from a tweet

        ~Processor(){}; //Destructor

        void update(Tweet const& tweet){}; //Updates queue when a tweet arrives
        void extract(Tweet const& tweet, Serie_Producer const& serial, Size_Producer const& sizal){}; //Extracts terminated cascades and publishes them
}
