### Project description
This is a tweetoscope project done as a part of the SDI-M software engineering course, which aims to develop a tweet popularity prediction application deployed on a set of K8S nodes. 
The different project nodes communicate using kafka, through a set of topics. 
The model is developed in Python, the incoming tweet treatment apps in c++. 

## Project components
* [Generator](#Generator)
* [Collector](#Collector)
* [Estimator](#Estimator)
* [Predictor](#Predictor)
* [Docker](#Docker)

## Generator
The tweet generator simulates the arrival in real time of tweets by reading them from files and posting them into a Kafka topic called tweets.
This is provided as a C++ source code for the project, and runs on one node.
# Running the generator
```
$ cd ../Generator/src
$ g++ -o ../tweet-generator -O3 -Wall -std=c++17 tweet-generator.cpp `pkg-config --libs --cflags gaml cppkafka` -lpthread
$ cd .. 
$ ./tweet-generator params.config
```
# Dependencies
the data.csv files

## Collector
The tweet collector gathers retweets and groups them into cascades, before delivering them to the subsequent processing stages
It outputs two types of messages to relevant topics : 
    * **cascade_series** : are partial time series of unfinished cascades, which are used to estimate the cascade's Hawkes params
    * **cascade_properties** : the final size of a terminated cascade, with relevant information. Used to train random forest models and compute prediction error
Coded in c++, using _gaml_ and _cppKafka_ library
# Running the generator
```
$ cd ../Collector/src
$ g++ -o ../tweet-collector -O3 -Wall -std=c++17 *.cpp `pkg-config --libs --cflags gaml cppkafka` -lpthread
$ cd .. 
$ ./tweet-collector collector.ini
```

## Estimator
Estimates the parameters of a Hawkes process given its partial time series. 
Time series are provided through the *cascade_series* topic, polled by a kafka consumer. 
Coded in Python using _numpy_ and _scipy_ 
# Running the generator
```
$ cd ../Estimator
$ python3 Estimator.py --broker-list 
```
# Arguments 
* **broker-list** : list of kafka brokers of interest, defaults to localhost::9092 

## Predictor
This is the predictor node which estimates the size of a cascade given its hawkes params. 
There are two prediction models at play :
    * Through the Hawkes process parameters we can estimate a cascade size per series, which constitues with the cascade params a _sample_ posted to the *samples* topic 
    * In the _learner_ node, a random forest model is trained for each observation window using the samples from the _samples_ topic. These trained models are then output to the *models* topic
    * The predictor uses the random forest models to make more accurate predictions, by reading them from the models topic 

Prediction stats (mainly errors) are sent to the _stats_ topic, new prediction create alerts on the _alerts_ topic, samples are forwarded to the _samples_ topic
# Running the predictor 
On separate nodes : 
    * Run the learner
```
$ cd ../Predictor
$ python3 learner.py --broker-list --batch-size
```
    * Run the predictor
```
$ cd ../Predictor
$ python3 predictor.py --broker-list --cascade-memory-size 
```
# Arguments 
* **broker-list** : list of kafka brokers of interest, defaults to localhost::9092 
* **batch-size** : number of samples to receive before retraining a forest, default=10
* **cascade-memory-size** : number of cascade series to store in memory before deleting older ones, default=1000

## Docker
Docker resources for the project :
    * *Dockerfile_gaml* : Dockerfile for creating a base image containing working project environment
    * *requirements.txt* : List of requirements for each node

The Dockerfiles for each project node is stored in its respective directory