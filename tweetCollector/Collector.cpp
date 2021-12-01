#include "SourceProcessor.hpp" 
#include "tweetoscopeConsumer.hpp"
#include "tweetoscopeProducer.hpp"
#include "CollectorParams.hpp"

#include <istream>

int main(int argc, char* argv[]) {

  // get params from config file
  if(argc != 2) {
    std::cout << "Usage : " << argv[0] << " <config-filename>" << std::endl;
    return 0;
  }
  tweetoscope::params::collector params(argv[1]);
  
  // Create Consumer
  tweetoscope::tweetConsumer tweetConsumer(params);
  tweetConsumer.subscribeTopic();

  // Create producers
  std::string outputType;
  outputType = "out_properties";
  tweetoscope::size_Producer sizal(params, outputType);
  outputType = "out_series";
  tweetoscope::serie_Producer serial(params, outputType);

  std::map<int,tweetoscope::Processor*> processor_map;

  while (true){
    // get message from topic 
    auto msg = tweetConsumer.poll();
    // testing if msg usable
    if (msg && !msg.get_error()){
        // transform msg as cascade_id, tweet
        tweetoscope::Tweet tweet;
        auto cascade_id = std::string(msg.get_key());
        if ( processor_map.find(tweet.source) == processor_map.end()) { //this source does not have a processor
          processor_map[tweet.source] = new tweetoscope::Processor(params, serial, sizal, tweet.source); //to each source its processor 
        }
        processor_map[tweet.source]->process(tweet,cascade_id); //we run the engine corresponding to this source
    }
  }

}
