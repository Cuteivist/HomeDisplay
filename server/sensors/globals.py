from sensors.door_sensor import DoorSensor
from sensors.temp_sensor import TemperatureSensor


def get_sensor_list():
    return [
        TemperatureSensor("temp_sensor_1"),  # bathroom bath
        TemperatureSensor("temp_sensor_2"),  # bathroom kitchen
        DoorSensor("door_sensor_1")  # door
    ]