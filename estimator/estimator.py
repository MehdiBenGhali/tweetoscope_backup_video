import json  # To parse and dump JSON
from kafka import KafkaConsumer  # Import Kafka consumer
from kafka import KafkaProducer  # Import Kafka producer
import numpy as np
import estimator_header
import logger
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--broker-list', type=str, help="the broker list", default="localhost:9092")
    args = parser.parse_args()  # Parse arguments

    logger = logger.get_logger('estimator', broker_list=args.broker_list, debug=True)
    consumer = KafkaConsumer("cascade_series",  # Topic name
                             bootstrap_servers=args.broker_list,  # List of brokers
                             value_deserializer=lambda v: json.loads(v.decode('utf-8')),
                             # How to deserialize the value from a binary buffer
                             key_deserializer=lambda v: v.decode()  # How to deserialize the key (if any)
                             )

    producer = KafkaProducer(
        bootstrap_servers=args.broker_list,  # List of brokers passed from the command line
        value_serializer=lambda v: json.dumps(v).encode('utf-8'),  # How to serialize the value to a binary buffer
        key_serializer=str.encode  # How to serialize the key
    )

    for msg in consumer:  # Blocking call waiting for a new message
        times = msg.value['times']
        history = [list(a) for a in zip(times, msg.value['magnitudes'])]
        history = np.array(history)
        _, params = estimator_header.compute_MAP(history, times[-1])
        text = msg.value['msg']
        cid = msg.value['cid']
        n_obs = len(times)
        n_supp, g1, n_star = estimator_header.prediction(params, history, times[-1])
        value = {
            'type': 'parameters',
            'cid': cid,
            'msg': text,
            'n_obs': n_obs,
            'n_supp': n_supp,
            'n_star': n_star,
            'G1': g1,
            'params': params.tolist()
        }
        logger.debug("parameters sent in cascade properties")
        producer.send('cascade_properties', key=str(msg.value['T_obs']), value=value)
