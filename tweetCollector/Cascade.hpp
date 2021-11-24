#pragma once

#include "SourceProcessor.hpp"
#include "Tweet.hpp"

#include <memory>
#include <vector> 
#include <string>
#include <boost/heap/binomial_heap.hpp>

class Cascade{
    private:

        std::string cascade_id;
        std::string message; //Initial tweet message
        std::vector<std::pair<int,int>> retweets; //Retweet magnitude and time pairs

    public:
        priority_queue::handle_type location; //Position in priority queue
        int last_tweet_time;

        Cascade() = default; 
        Cascade(const Cascade&) = default;
        Cascade& operator=(const Cascade&) = default; //Constructors

        Cascade(Tweet const& tweet) : 
        cascade_id(tweet.cascade_id), message(tweet.message), 
        retweets(std::make_pair(tweet.magnitude, tweet.time)), 
        last_tweet_time(tweet.time) {}; //Constructor from a tweet

        inline bool operator<(const Cascade& other) const { return (this->last_tweet_time < other.last_tweet_time)} //Compares cascades by last tweet update time
        inline void update(const int& magnitude, const int& time) {retweets.pushback(std::make_pair(magnitude, time))} //Updates a cascade with an incoming tweet
        inline bool isAlive(Tweet const& tweet) {return ((tweet.time - this->last_tweet_time)<seuil)}              
}
