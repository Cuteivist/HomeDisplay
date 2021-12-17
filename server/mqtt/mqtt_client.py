import paho.mqtt.client as mqtt
from sensors.globals import get_sensor_list


class MqttClient:
    def __init__(self):
        self.MQTT_MAIN_TOPIC = "zigbee2mqtt"
        self.sensor_list = get_sensor_list()
        for sensor in self.sensor_list:
            sensor.run_db_transitions()
        self.mqtt_client = mqtt.Client(client_id="DisplayMqttClient")
        self.mqtt_client.username_pw_set('', '')  # Username, password
        self.mqtt_client.on_connect = self.on_connect
        self.mqtt_client.on_message = self.on_message
        self.mqtt_client.connect('pi.lan', 1883)
        self.mqtt_client.loop_forever()

    def on_connect(self, client, userdata, flags, rc):
        print('Connected with result code ' + str(rc))
        client.subscribe("#")

    def on_message(self, client, userdata, msg):
        print(f"Received message from topic: '{msg.topic}'")
        for sensor in self.sensor_list:
            if msg.topic == self.MQTT_MAIN_TOPIC + "/" + sensor.name:
                sensor.update(msg.payload)
                return


if __name__ == '__main__':
    mqtt_client = MqttClient()
