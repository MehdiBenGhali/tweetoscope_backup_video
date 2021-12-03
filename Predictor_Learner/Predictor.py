import json   # To parse and exchange in JSON format
import numpy as np
import pickle
from kafka import KafkaConsumer  # Import Kafka consumer
from kafka import KafkaProducer  # Import Kafka producer
import logger
import argparse


class DefaultForest:  # This replaces the forest when we don't have trained a forest yet
    @staticmethod
    def predict(x):  # returns W default
        return [1]


class PredictorHandler:

    def __init__(self, consumer, producer, logger, cascade_memory_size):
        self.consumer = consumer
        self.producer = producer
        self.models = {}  # stores the models sent by the learner
        self.partial_parameters = {}  # stores the parameters of partial cascades
        self.complete_cascades = {}  # stores the parameters of complete cascades
        # queue storing the cascade ids of complete cascades in order of arrival
        self.cascade_ids_of_complete_cascades = []
        self.logs = logger
        # maximum amount of cascades stored
        self.cascade_memory_size = cascade_memory_size

    def compute_w(self, n_inf, n_current, g1, n_star):  # computes the target W (Hawkes)
        if g1 == 0:
            self.logs.warning("g1 = 0. Replacing with 1")
            g1 = 1
        return (n_inf - n_current) * (1 - n_star) / g1

    @staticmethod
    def predict(n_current, g1, n_star, w_value=1):
        return n_current + w_value * g1 / (1 - n_star)

    @staticmethod
    def deserializer(v):  # the deserializer depends on the topic. If it's not json, it's pickle
        try:
            return json.loads(v.decode('utf-8'))
        except UnicodeDecodeError:
            return pickle.loads(v)

    def post_samples_and_stats(self):
        for cascade_id in self.complete_cascades.keys():  # We go through terminated cascades
            msg = self.complete_cascades[cascade_id]
            # For each partial cascade corresponding to a terminated one
            while self.partial_parameters.get(cascade_id, []):
                msg_casc_part = self.partial_parameters[cascade_id].pop()
                w = self.compute_w(msg.value['n_tot'], msg_casc_part.value['n_obs'],  # Compute target cascade_size (W)
                                   msg_casc_part.value['G1'], msg_casc_part.value['n_star'])
                value = {
                    'type': 'sample',
                    'cascade_id': cascade_id,
                    'X': [msg_casc_part.value['params'][1], msg_casc_part.value['n_star'], msg_casc_part.value['G1']],
                    'W': w
                }
                self.logs.debug("sample sent")
                self.producer.send(
                    'samples', key=msg_casc_part.key, value=value)
                err = np.abs(
                    (msg_casc_part.value['n_supp'] - msg.value['n_tot']) / msg.value['n_tot'])
                value = {'type': 'stat', 'cascade_id': cascade_id,
                         'T_obs': msg_casc_part.key, 'Err': err}
                self.logs.debug("stats sent")
                self.producer.send('stats', key=msg_casc_part.key, value=value)

    def update_models(self, msg):
        self.models[msg.key] = msg.value
        self.logs.info("updated model for window {}".format(msg.key))

    def predict_and_store(self, msg):
        cascade_id = msg.value['cascade_id']
        if msg.value['type'] == 'parameters':  # If partial cascade
            # Either get an existing or use a default model for this time window
            forest = self.models.get(msg.key, DefaultForest)

            # We predict n_tot using our random forest
            n_supp = self.predict(msg.value['n_obs'], msg.value['G1'], msg.value['n_star'],
                                  forest.predict([[msg.value['params'][1], msg.value['n_star'], msg.value['G1']]])[0])
            value = {
                'type': 'alert',
                'cascade_id': cascade_id,
                'msg': msg.value.get('msg'),
                'T_obs': msg.key,
                'n_tot': n_supp
            }
            self.logs.debug("alert sent")
            self.producer.send('alerts', key=msg.key, value=value)
            msg.value['n_supp'] = n_supp  # We update the predicted value
            if cascade_id in self.parameters.keys():  # We save the hawkes params from this partial cascade
                self.parameters[cascade_id].append(msg)
            else:
                self.parameters[cascade_id] = [msg]

        else:  # If terminated cascade
            # If cascade memory limit is attained
            if len(self.cascade_ids_of_complete_cascades) >= self.cascade_memory_size:
                self.logs.info("cascade memory is full")
                # Delete the oldest cascade
                del self.complete_cascades[self.cascade_ids_of_complete_cascades.pop(
                    0)]
            if cascade_id not in self.cascade_ids_of_complete_cascades:
                # We add the cascade_id at the end of the queue
                self.cascade_ids_of_complete_cascades.append(cascade_id)
                self.complete_cascades[cascade_id] = msg


if __name__ == "__main__":
    ############ Parse arguments passed from the command line ############
    parser = argparse.ArgumentParser()
    parser.add_argument('--broker-list', type=str,
                        default="kafka-service:9092")  # kafka broker list
    # maximum cascades to store in memory
    parser.add_argument('--cascade-memory-size', type=int, default=1000)
    args = parser.parse_args()  # Parse arguments

    ############ Create and setup kafka objects ############
    logs = logger.get_logger(
        'predictor', broker_list=args.broker_list, debug=True)

    consumer = KafkaConsumer(
        bootstrap_servers=args.broker_list,  # List of brokers
        value_deserializer=lambda v: PredictorHandler.deserializer(
            v),  # Deserialize the payload from a binary buffer
        key_deserializer=lambda v: v.decode()  # Deserialize the key (if any)
    )
    producer = KafkaProducer(
        bootstrap_servers=args.broker_list,  # List of brokers
        value_serializer=lambda v: json.dumps(v).encode(
            'utf-8'),  # Serialize the value to a binary buffer
        key_serializer=str.encode  # Serialize the key
    )
    consumer.subscribe(["cascade_properties", "models"])

    ############ Create a class instance to handle predictors ############
    predictor_handler = PredictorHandler(
        consumer, producer, logs, args.cascade_memory_size)

    ############ Main Loop ############
    for msg in consumer:
        if msg.topic == "models":  # If we received a model
            predictor_handler.update_models(msg)
            continue
        predictor_handler.predict_and_store(msg)
        predictor_handler.post_samples_and_stats()
