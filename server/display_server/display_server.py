import json
import argparse
import re
import sqlalchemy as db
import os
from http.server import HTTPServer, BaseHTTPRequestHandler
import time
import datetime
import configparser

class DisplayServerHTTPRequestHandler(BaseHTTPRequestHandler):
    def normalize_datetime_array(self, array):
        array_len = len(array)
        if array_len == 0:
            return list()
        elif array_len == 1:
            return [0]
        # Start from 0 and go to 1
        result = [0]
        last_element = array[-1]
        first_element = array[0]
        diff = (array[-1] - array[0]).total_seconds()
        for d in array[1:]:
            result.append((d - first_element).total_seconds() / diff)
        return result

    def remove_duplicates(self, array1, array2):
        length = len(array1) - 1
        if length < 1 or len(array2) != len(array1):
            return
        # Only two duplicates are neede for drawing
        # In case values are [2, 2, 2, 3, 4]
        # Middle 2 isn't needed, but those 2 are 
        # required to draw flat line
        same_value_count = 1
        for i in reversed(range(length)):
            if array1[i] == array1[i + 1]:
                if same_value_count > 1:
                    del array1[i + 1]
                    del array2[i + 1]
                same_value_count += 1
            else:
                same_value_count = 1
                last_value = array1[i]


    def do_GET(self):
        self.send_response(200)
        self.end_headers()
        data_json = {
            "time": datetime.datetime.now().strftime("%H:%M"),
            "small_plots": list()
        }
        start_time = time.time()
        db_engine = db.create_engine(f"sqlite:///{os.getenv('HA_DB_PATH')}")
        db_connection = db_engine.connect()
        db_metadata = db.MetaData()
        states_table = db.Table('states', db_metadata, autoload=True, autoload_with=db_engine)
        filter_time = datetime.datetime.utcnow() - datetime.timedelta(hours=24)
        query = db.select([states_table.columns.entity_id, states_table.columns.state, states_table.columns.last_updated]).where(db.and_(states_table.columns.domain == "sensor", states_table.columns.last_updated > filter_time)).order_by(db.asc(states_table.columns.last_updated))
        result_set = db_connection.execute(query).fetchall()
        print(f"Fetched {len(result_set)} results in {round(time.time() - start_time, 4)} secs")
        start_time = time.time()

        # Parase DB data
        sensor_dict = dict()
        for result in result_set:
            sensor_name = result[0]
            if not re.match("^sensor\.temp.*(temperature|humidity)$", sensor_name):
                continue
            sensor_state = result[1]
            sensor_state_date = result[2]
            if sensor_name not in sensor_dict:
                sensor_dict[sensor_name] = {
                    "x": list(),
                    "y": list()
                }
            sensor_dict[sensor_name]["x"].append(sensor_state_date)
            sensor_dict[sensor_name]["y"].append(sensor_state)

        # Extract sensors mentioned in config
        for small_plot_config in self.server.config_small_plots:
            sensor_entity = "sensor." + small_plot_config["entity"]
            if sensor_entity not in sensor_dict:
                print(f"ERROR SENSOR {sensor_entity} NOT FOUND!")
                continue
            sensor_obj = sensor_dict[sensor_entity]
            # Normalize list to be from 0 to 1
            sensor_obj['x'] = self.normalize_datetime_array(sensor_obj['x'])
            # Remove dupliates
            self.remove_duplicates(sensor_obj['y'], sensor_obj['x'])
            data_json["small_plots"].append({
                "title": small_plot_config["name"],
                "x": sensor_obj['x'],
                "y": sensor_obj['y'],
                "yLabels": ["20", "21", "22", "23", "24", "25"], # TODO calculate axis labels
                "xLabels": ["", "15:30", "05:00", ""], # TODO get correct time
                "xMax": 1,
                "xMin": 0,
                "yMax": 25,
                "yMin": 20
            })
        
        print(f"Parsed data in: {round(time.time() - start_time, 4)} secs")
        self.wfile.write(json.dumps(data_json).encode("utf-8"))


class DisplayServer:
    def __init__(self, port):
        ip = "0.0.0.0"
        if port == None:
            port = 8881
        print(f"Starting server at address: {ip}:{port}")
        config = configparser.ConfigParser()
        config.read(os.getenv('SERVER_CONFIG_FILE'))
        httpd = HTTPServer((ip, port), DisplayServerHTTPRequestHandler)
        httpd.config_small_plots = json.loads(config.get("Global", "small_plots"))
        httpd.serve_forever()


if __name__ == '__main__':
    # Instantiate the parser
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('-p', type=int, help='Port')
    args = parser.parse_args()
    display_server = DisplayServer(args.p)
