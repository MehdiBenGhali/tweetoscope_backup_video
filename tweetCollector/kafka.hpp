#pragma once

#include "cascade.hpp"

#include <cppkafka/cppkafka.h>
#include <vector> 
#include <string>
#include <memory>

using namespace cppkafka ;

using wref_cascade = std::weak_ptr<Cascade> ;

struct Serie_Producer {
    private:
        MessageBuilder builder;
        Producer producer; 
        std::string& from_cascade(wref_cascade const& cascade){}; //Converts a cascade to a kafka series payload

    public:
        serie_producer() = default; 
        serie_Producer(const serie_Producer&) = default;
        serie_producer& operator=(const serie_Producer&) = default;
        serie_Producer(std::string& topic_name, Configuration& config); //Constructor from builder and producer params

                
        void produce(wref_cascade const& cascade){}; //Produces a cascade as a tweet series
};

using times = std::vector<int> ;

struct Size_Producer {
    private:
        MessageBuilder builder;
        Producer producer; 
        std::string& from_cascade(wref_cascade const& cascade){}; //Converts a cascade to a kafka series payload

    public:
        serie_producer() = default; 
        serie_Producer(const serie_Producer&) = default;
        serie_producer& operator=(const serie_Producer&) = default;
        serie_Producer(std::string& topic_name, Configuration& config); //Constructor from builder and producer params

        void produce(wref_cascade const& cascade, times const& keys){}; //Produces a cascade size message for each predictor
};
