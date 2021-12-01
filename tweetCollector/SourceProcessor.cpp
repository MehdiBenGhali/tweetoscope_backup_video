#include "SourceProcessor.hpp"

#include <memory>
#include <boost/heap/binomial_heap.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

tweetoscope::Processor::Processor(tweetoscope::params::collector params,tweetoscope::serie_Producer& serial,
    tweetoscope::size_Producer& sizal, int collection_source) :
        sizal(sizal),
        serial(serial),
        expiration_time(params.times.terminated),
        timewindows(params.times.observation),
        this_collection_source(collection_source),
        min_cascade_size(params.cascade.min_cascade_size)
        {
            // we initialize an empty map
            for (auto iter=timewindows.begin(); iter!=timewindows.end(); ++iter){ 
                std::queue<tweetoscope::wref_cascade> empty_queue;
                partial_cascade_map[*iter] = empty_queue;
            }
        }


//Extracts terminated cascades and publishes them
void tweetoscope::Processor::extractExpired(tweetoscope::Tweet const& tweet){
    while(!cascade_queue.empty()) {
        auto ref = cascade_queue.top();
        if (!(ref->isAlive(tweet))){
            int t_end = ref->last_tweet_time;
            int size = (ref->times).size();
            if((ref->times).size()>=min_cascade_size){
                serial.post(ref, t_end);
                sizal.post(ref, timewindows);
            }
            cascade_queue.pop(); //Remove terminated cascade from queue
            ref.reset(); //Decrement cascade pointer
        } else { 
            break;
        }
    }
}

//Post partial active cascades on time of tweet
void tweetoscope::Processor::postPartials(tweetoscope::Tweet const& tweet){
    for (auto iter=timewindows.begin(); iter!=timewindows.end(); ++iter){
        auto partial_queue = partial_cascade_map[*iter] ;
        while(!(partial_queue.empty()) && (partial_queue.front().lock()->isAlive(tweet))
            && (((partial_queue.front().lock())->times).front() + *iter)<=tweet.time) {
                auto wrcascade = partial_queue.front();
                partial_queue.pop();
                if (wrcascade.lock()) {
                    serial.post(wrcascade.lock(),*iter);
                }              
            }
        }
    }

//create new cascade or update existing one 
void tweetoscope::Processor::updateCascades(tweetoscope::Tweet const& tweet, std::string const& cascade_id){
    if (cascades_map.find(cascade_id)==cascades_map.end() && tweet.type=="tweet"){ // New Cascade
        tweetoscope::ref_cascade new_casc = std::make_shared<tweetoscope::tweetCascade>(cascade_id,tweet,expiration_time);
        ///new_casc->setLocation(cascade_queue.push(new_casc)); // Add cascade to priority queue  ""!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!""
        cascade_queue.update(new_casc->location, new_casc);  // Update priority queue with right value
        cascades_map[cascade_id] = new_casc;                         // Add cascade to map
        for (auto iter=timewindows.begin(); iter!=timewindows.end(); ++iter){
            partial_cascade_map[*iter].push(new_casc); //Add partial cascades of cascade to map
        }
    } else if (!(cascades_map.find(cascade_id)==cascades_map.end()) && tweet.type=="retweet") { //Retweet Update 
        if(cascades_map[cascade_id].lock()->isAlive(tweet)){                        // if cascade not expired
            auto this_casc = cascades_map[cascade_id].lock();      // lock weak_ptr
            this_casc->update(tweet);                              // update the cascade
            cascade_queue.update(this_casc->location, this_casc);  // update the priority queue
        } else {
            cascades_map.erase(cascade_id);                        // if expired, remove from ref map
            ///partial_cascade_map.erase(cascade_id);                    // and remove partial cascades """"""""""""""""""""""!!!!!!!!!!!!!!!!!!!!!!!!!!!
        }
    }  
}

//Run processor treatments upon tweet arrival
void tweetoscope::Processor::process(tweetoscope::Tweet const& tweet, std::string const& cascade_id){
    if(tweet.source == this_collection_source) {
        //Extract expired cascades
        this->extractExpired(tweet);

        //Publish partial casades 
        this->postPartials(tweet);

        //Update Cascades
        this->updateCascades(tweet, cascade_id);
    }
} 
