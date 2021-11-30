#pragma once

#include "SourceProcessor.hpp"
#include "Tweet.hpp"

#include <memory>
#include <vector> 
#include <string>
#include <boost/heap/binomial_heap.hpp>

namespace tweetoscope {
class Cascade;

using ref_cascade = std::shared_ptr<Cascade>;

class ref_cascade_comparator {
    inline bool operator()(ref_cascade op1, ref_cascade op2) const {return *op1 < *op2;} 
}

using priority_queue = boost::heap::binomial_heap<ref_cascade, boost::heap::compare<ref_cascade_comparator>>;

class Cascade{
    private:
        std::string message; //Initial tweet message
        std::vector<double> magnitudes; //Retweet times vector
        std::vector<double> times; //Retweet magnitudes vector

    public:
        std::string cascade_id; //Identifiant de la cascade
        priority_queue::handle_type location; //Position in priority queue
        double last_tweet_time;

        Cascade() = default; 
        Cascade(const Cascade&) = default;
        Cascade& operator=(const Cascade&) = default; //Constructors

        Cascade(Tweet const& tweet) : 
        cascade_id(tweet.cascade_id), message(tweet.message), 
        magnitudes(std::vector<double>(tweet.magnitude)), times(std::vector<double>(tweet.time)),
        last_tweet_time(tweet.time) {}; //Constructor from a tweet

        inline bool operator<(const Cascade& other) const { return (this->last_tweet_time < other.last_tweet_time);} //Compares cascades by last tweet update time
        //Updates a cascade with an incoming tweet
        inline void update(Tweet const& tweet) {
            magnitudes.pushback(tweet.magnitude);
            times.pushback(tweet.time);
            last_tweet_time = tweet.time ;
            } 
        inline bool isAlive(Tweet const& tweet) {return ((tweet.time - this->last_tweet_time)<seuil);}   

        std::string toSeries(double time); //Creates a JSON representation of the cascade time series at a given time   
        std::string toSize(); //Creates a JSON representation of a terminated cascade size 

        friend std::ostream& operator<<(std::ostream& os, Cascade const& cascade);         
}
}