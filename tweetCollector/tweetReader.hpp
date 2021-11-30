#pragma once

#include "tweet.hpp"

#include <string>
#include <cstddef>
#include <iostream>

namespace tweetoscope {

  inline std::string get_string_val(std::istream& is) {
    char c;
    is >> c; // eats  "
    std::string value;
    std::getline(is, value, '"'); // eats tweet", but value has tweet
    return value;
  }

  inline std::istream& operator>>(std::istream& is, tweetoscope::Tweet& tweet) {
    // A tweet is  : {"type" : "tweet"|"retweet",
    //                "msg": "...",
    //                "time": timestamp,
    //                "magnitude": 1085.0,
    //                "source": 0,
    //                "info": "blabla"}
    std::string buf;
    char c;
    is >> c; // eats '{'
    is >> c; // eats '"'
    while(c != '}') {
      std::string tag;
      std::getline(is, tag, '"'); // Eats until next ", that is eaten but not stored into tag.
      is >> c;  // eats ":"
      if     (tag == "type")    tweet.type = get_string_val(is);
      else if(tag == "msg")     tweet.msg  = get_string_val(is);
      else if(tag == "info")    tweet.info = get_string_val(is);
      else if(tag == "t")       is >> tweet.time;
      else if(tag == "m")       is >> tweet.magnitude;
      else if(tag == "source")  is >> tweet.source;

      is >> c; // eats either } or ,
      if(c == ',')
        is >> c; // eats '"'
    }
    return is;
  }
}
