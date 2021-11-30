import argparse  # To parse command line arguments
import json  # To parse and dump JSON
from kafka import KafkaConsumer  # Import Kafka consumer

# logger = logger.get_logger('my-node', broker_list='localhost:9092', debug=True)  # the source string (here 'my-node') helps to identify
# in the logger terminal the source that emitted a log message.

# logger.critical("Houston, we have a problem.")       # CRITICAL errors abort the application

parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('--broker-list', type=str, required=True, help="the broker list")
parser.add_argument('--topic', type=str, required=True, help="the topic name")
args = parser.parse_args()  # Parse arguments

consumer = KafkaConsumer(args.topic,  # Topic name
                         bootstrap_servers=args.broker_list,  # List of brokers passed from the command line
                         value_deserializer=lambda v: json.loads(v.decode('utf-8')),
                         # How to deserialize the value from a binary buffer
                         key_deserializer=lambda v: v.decode()  # How to deserialize the key (if any)
                         )

for msg in consumer:  # Blocking call waiting for a new message
    print(f"msg: ({msg.key}, {msg.value})")  # Write key and payload of the received message
    #logger.error("What you ask me is just nonsense!")  # recoverable ERROR
