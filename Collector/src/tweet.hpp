#pragma once

#include <memory>
#include <vector> 
#include <string>

namespace tweetoscope{
struct Tweet{
    //Attributes
    std::string type;
    double source;
    std::string message;
    int time;
    int magnitude; 
    std::string info;

    //Constructors
    Tweet() = default;
    Tweet(const Tweet&) = default;
    Tweet& operator=(const Tweet&) = default; //Default constructors
    Tweet(std::string const& type,double const& source,std::string const& message,int t, int m) :
    type(type), source(source), message(message), time(t), magnitude(m) {} //Element Constructor
};
}