import json  # To parse and exchange in JSON format
from kafka import KafkaConsumer  # Import Kafka consumer
from kafka import KafkaProducer  # Import Kafka producer
import pandas as pd
from sklearn.ensemble import RandomForestRegressor
import pickle
import argparse
import logger

if __name__ == "__main__":
    ################# Argument parsing #################
    parser = argparse.ArgumentParser()
    # Broker list provided as argument
    parser.add_argument('--broker-list', type=str, default="localhost:9092")
    # The amount of new cascade samples to train on
    parser.add_argument('--batch-size', type=int, default=10)
    args = parser.parse_args()  # Parse arguments

    ################# Creating a kafka consumer #################
    consumer = KafkaConsumer("samples",  # Topic name
                             bootstrap_servers=args.broker_list,  # List of brokers
                             value_deserializer=lambda val: json.loads(
                                 val.decode('utf-8')),  # Deserialize the message payload
                             key_deserializer=lambda key: key.decode()  # Deserialize the message key
                             )

    ################# Creating a kafka producer #################
    producer = KafkaProducer(
        # List of brokers passed from the command line
        bootstrap_servers=args.broker_list,
        value_serializer=lambda val: pickle.dumps(
            val),  # Serialize the message payload
        key_serializer=str.encode  # Serialize the key
    )

    ################# Get the kafka logger #################
    logger = logger.get_logger(
        'predictor', broker_list=args.broker_list, debug=True)

    # We need to keep track of multiple models each assigned to an observation window
    # Each window contains a randomforest and a Hawkes model
    # We use a dictionary with obs time keys to do so
    model_dict = {}
    # Batch size is user input
    batch_size = args.batch_size
    # A default dictionnary, which is returned if no model corresponding to current key is found
    default_dict = {
        'Hawkes_params': pd.DataFrame(columns=['beta', 'n_star', 'G1', 'W']),
        'regressor': RandomForestRegressor(),
        'cascade_count': 0
    }

    ################# Main loop #################
    for msg in consumer:
        # On message arrival, get the corresponding models dict
        work_dico = model_dict.get(msg.key, default_dict)
        # Add Hawkes estimator for the current observation
        work_dico['Hawkes_params'] = work_dico['Hawkes_params'].append({'beta': msg.value['X'][0],
                                                                        'n_star': msg.value['X'][1],
                                                                        'G1': msg.value['X'][2],
                                                                        'W': msg.value['W']},
                                                                       ignore_index=True)
        # Set the new models dict
        model_dict[msg.key] = work_dico
        # Forest regressor treatment
        if model_dict[msg.key]['cascade_count'] < batch_size:
            # Increment cascade count if batch isn't due for training
            model_dict[msg.key]['counter'] += 1
        # If batch size is big enough for training
        elif model_dict[msg.key]['cascade_count'] >= batch_size:
            model_dict[msg.key]['counter'] = 0  # Reset counter
            # Prepare data for training
            X = model_dict[msg.key]['dataframe'][['beta', 'n_star', 'G1']]
            y = model_dict[msg.key]['dataframe']['W']
            # Fit the model
            model_dict[msg.key]['regressor'].fit(X, y)
            # Post model to "models" topic
            producer.send('models', key=msg.key,
                          value=model_dict[msg.key]['regressor'])
            logger.debug("trained model for window {}".format(
                msg.key))  # Log info
