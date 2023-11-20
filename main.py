import json
import requests
import paho.mqtt.client as mqtt

# MQTT Broker Configuration
MQTT_BROKER = "172.20.10.2"
MQTT_TOPIC = "dht11"

# JSON Server Configuration
JSON_SERVER_URL = "http://172.20.10.2:3000/dht11"

def on_connect(client, userdata, flags, rc):
    print(f"Connected to MQTT Broker with result code {rc}")
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    payload = msg.payload.decode("utf-8")
    print(f"Received message on topic {msg.topic}: {payload}")

    data = {"topic": msg.topic, "payload": payload}
    send_to_json_server(data)

def send_to_json_server(data):
    try:
        response = requests.post(JSON_SERVER_URL, data=json.dumps(data), headers={"Content-Type": "application/json"})
        response.raise_for_status()
        print("Data sent to JSON server successfully.")
    except requests.RequestException as e:
        print(f"Failed to send data to JSON server: {e}")

def main():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(MQTT_BROKER, 1883, 60)
    client.loop_start()

    try:
        # Keep the script running
        while True:
            pass
    except KeyboardInterrupt:
        print("Exiting...")
        client.disconnect()
        client.loop_stop()

if __name__ == "__main__":  
    main()
