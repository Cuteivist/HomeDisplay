import json
import logging
import traceback


class BaseSensor:
    def __init__(self, name):
        if not name:
            print("ERROR sensor has empty name")
            return
        self.name = name
        self.data = dict()
        for prop in self.get_property_list():
            self.data[prop] = list()
        print(f"Created sensor '{self.name}' with properties '{self.get_property_list()}")

    def get_property_list(self):
        return []

    def run_db_transitions(self):
        # TODO
        print("TODO add transitions")
        pass

    def update(self, payload):
        new_data_row = dict()
        try:
            payload_json = json.loads(str(payload.decode('utf-8')))
            for prop, value in payload_json.items():
                new_data_row[prop] = value
            print(f"Parsed '{self.name}' sensor new data row: {new_data_row}")
        except Exception as e: # TODO specify exception
            print(f"ERROR while trying to parse payload for sensor '{self.name}'. ")
            logging.error(traceback.format_exc())
            return
        print(f"TODO: Implement saving new data to DB")

    def tostring(self):
        return self.get_data()

    def load_data_from_db(self):
        # TODO
        print(f"TODO: Implement loading data from table {self.name}")
