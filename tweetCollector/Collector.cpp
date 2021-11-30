#include "SourceProcessor.hpp" 
#include "tweetoscopeConsumer.hpp"
#include "tweetoscopeProducer.hpp"
#include "CollectorParams.hpp"

int main(int argc, char* argv[]) {

  // get params from config file
  if(argc != 2) {
    std::cout << "Usage : " << argv[0] << " <config-filename>" << std::endl;
    return 0;
  }
  tweetoscope::params::collector params(argv[1]);
  
  // Create Consumer
  tweetoscope::tweetConsumer tweetConsumer(params);
  consumer.subscribeTopic();

  // Create producers
  tweetoscope::size_Producer sizal(params, "out_properties");
  tweetoscope::serie_Producer  serial(params, "out_series");

  std::map<int,tweetoscope::Processor*> processor_map;

  while (true){
    // get message from topic 
    auto msg = consumer.poll();
    // testing if msg usable
    if (msg && !msg.get_error()){
        // transform msg as cascade_id, tweet
        tweetoscope::Tweet tweet;
        auto cascade_id = std::string(msg.get_key());
        auto istr = std::istringstream(std::string(msg.get_payload()));
        istr >> tweet;
        if ( processor_map.find(tweet.source) == processor_map.end()) { //this source does not have a processor
          processor_map[tweet.source] = new tweetoscope::Processor(params, serial, sizal, tweet.source); //to each source its processor 
        }
        processor_map[tweet.source]->process(tweet,cascade_id); //we run the engine corresponding to this source
    }
  }

}
