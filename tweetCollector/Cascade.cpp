#include "cascade.hpp"

#include <memory>
#include <vector> 
#include <string>
#include <boost/heap/binomial_heap.hpp>

std::string tweetoscope::tweetCascade::toSeries(double time){
    auto it_m = magnitudes.begin();
    std::string list_times = "[";
    std::string list_magnitudes = "[";
    for (auto it_t =times.begin(); it_t!=times.end(); ++it_t, ++it_m){
        list_times = list_times + std::to_string(*it_t) + ",";
        list_magnitudes = list_magnitudes + std::to_string(*it_m) + ",";
    }
    list_times = list_times.substr(0, list_times.size()-1) + "]";
    list_magnitudes = list_magnitudes.substr(0, list_magnitudes.size()-1) + "]";
    return "{\"type\": \"serie\", \"cid\":" + std::to_string(cascade_id) + "," + "\"msg\":"+ message
    +"," + "\"T_obs\":"+ std::to_string(time) + "," + 
    "\"times\":" + list_times + "," +"\"magnitudes\":" + list_magnitudes + "}";
}

std::string tweetoscope::tweetCascade::toSize(){
    return "{\"type\": \"size\", \"cid\":" + std::to_string(cascade_id) + ","
    + "\"n_tot\":" + ///to do size
    + "," + "\"t_end\":" + std::to_string(last_tweet_time) + "}";
}

std::ostream& tweetoscope::operator<<(std::ostream & os, tweetoscope::tweetCascade const& cascade){
    os << "Id : " << cascade.cascade_id <<" | message initial: "<< cascade.message << std::endl;
    auto it_t = cascade.times.begin();
    for (auto it_m = cascade.magnitudes.begin() ; it_m != cascade.magnitudes.end(); ++it_m, ++it_t){
        os << "magnitude : " << (*it_m) << " | time : " << (*it_t) << std::endl;
    }
    os << std::endl;
    return os;
}