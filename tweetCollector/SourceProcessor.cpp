#pragma once

#include "SourceProcessor.hpp"

#include <memory>
#include <boost/heap/binomial_heap.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

tweetoscope::Processor::Processor(tweetoscope::params::collector params,tweetoscope::serie_producer& serial,
    tweetoscope::size_producer& sizal, int collection_source):
        sizal(sizal),
        serial(serial),
        expiration_time(params.times.terminated),
        times(params.times.observation),
        this_coll_source(coll_source),
        min_cascade_size(params.cascade.min_cascade_size)
        {
            // we initialize an empty map
            for (auto iter=times.begin(); iter!=times.end(); ++iter){ 
                std::queue<wref_cascade> empty_queue;
                partial_cascade_map[*iter] = empty_queue;
            }
        }


//Updates queue when a tweet arrives
void tweetoscope::Processor::update(Tweet const& tweet){
    auto begin = queue.begin();
    auto end = queue.end(); 
    for (auto it = begin; it!=end; it++) {
        if ((*it)->cascade_id==tweet.cascade_id)){
            (*it)->update(tweet); 
            break;
        } 
    }
}

//Extracts terminated cascades and publishes them
void tweetoscope::Processor::extract(Tweet const& tweet){
    while(!queue.empty()) {
        auto ref = queue.top();
        if (!(ref->isAlive(tweet)){
            t_end = (ref->last_tweet_time).back();
            size = (ref->times).size();
            if((ref->times).size()>=min_cascade_size){
                serial.produce(ref, t_end);
                sizal.produce(ref, times);
            }
            queue.pop(); //Remove terminated cascade from queue
            ref.reset() //Decrement cascade pointer
        } else { 
            break;
        }
    }
}
