import json

from sensors.base_sensor import BaseSensor


class TemperatureSensor(BaseSensor):
    def __init__(self, name):
        BaseSensor.__init__(self, name)

    def get_property_list(self):
        return ["battery", "humidity", "linkquality", "pressure", "temperature", "voltage"]
