#pragma once

#include <memory>
#include <vector> 
#include <string>

struct Tweet{
    std::string type;
    std::string message;
    double source;
    int time;
    int magnitude; 
    std::string info;

    Tweet() = default; 
    Tweet(const Tweet&) = default;
    Tweet& operator=(const Tweet&) = default; //Default constructors
    Tweet(std::string const& cascade_id,std::string const& type,std::string const& source,std::string const& message,int t, int m) :
    cascade_id(cascade_id), type(type), source(source), message(message), time(t), magnitude(m) {} //Element Constructor
}