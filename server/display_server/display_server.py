import json
import argparse
import re
from types import NoneType
import sqlalchemy as db
import os
from http.server import HTTPServer, BaseHTTPRequestHandler
import time
import datetime
import configparser
import math
import requests
from decimal import Decimal
from urllib.parse import urlparse

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
            result.append(f"{((d - first_element).total_seconds() / diff):.3f}")
        return result

    def remove_duplicates(self, array1, array2):
        length = len(array1) - 1
        if length < 1 or len(array2) != len(array1):
            return

        # If sensor was unavaialble there will be null values
        for i in reversed(range(length)):
            val = array1[i]
            if type(val) is NoneType or val == "unknown" or val == "unavailable":
                del array1[i]
                del array2[i]

        length = len(array1) - 1
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

        
    def nice_number(self, value, round_=False):
        exponent = math.floor(math.log(value, 10))
        fraction = value / 10 ** exponent

        if round_:
            if fraction < 1.5:
                nice_fraction = 1.
            elif fraction < 3.:
                nice_fraction = 2.
            elif fraction < 7.:
                nice_fraction = 5.
            else:
                nice_fraction = 10.
        else:
            if fraction <= 1:
                nice_fraction = 1.
            elif fraction <= 2:
                nice_fraction = 2.
            elif fraction <= 5:
                nice_fraction = 5.
            else:
                nice_fraction = 10.

        return nice_fraction * 10 ** exponent

    def nice_bounds(self, axis_start, axis_end, num_ticks=10):
        axis_width = axis_end - axis_start
        if axis_width == 0:
            nice_tick = 0
        else:
            nice_range = self.nice_number(axis_width)
            nice_tick = self.nice_number(nice_range / (num_ticks - 1), round_=True)
            axis_start = math.floor(axis_start / nice_tick) * nice_tick
            axis_end = math.ceil(axis_end / nice_tick) * nice_tick

        return axis_start, axis_end, nice_tick

    def remove_exponent(self, num):
        decimal_num = Decimal(num)
        return decimal_num.quantize(Decimal(1)) if decimal_num == decimal_num.to_integral() else decimal_num.normalize()


    def create_plot_obj(self, location, plot_type, title, x_series, y_series, plot_time):
        is_forecast = plot_type == "forecast"
        plot_obj = {
            "title": title,
            "location": location,
            "type": plot_type,
            "xMin": 0,
            "xMax": 1,
            "x": x_series,
            "y": y_series
        }
        min_y = 9999.0
        max_y = -9999.0
        # Find min and max of all series
        for series in y_series:
            if type(series) == list:
                for val in series:
                    casted_val = float(val)
                    min_y = min(min_y, casted_val)
                    max_y = max(max_y, casted_val)
            else:
                casted_val = float(series)
                min_y = min(min_y, casted_val)
                max_y = max(max_y, casted_val)

        if is_forecast:
            # For forecast we want to show rain at the bottom, 
            # which require givime more space to not overlap bar chart with plot
            diff = max_y - min_y
            min_y -= diff / 2

        (nice_min_y, nice_max_y, nice_tick_y) = self.nice_bounds(min_y, max_y, 5)

        plot_obj["yMin"] = nice_min_y
        plot_obj["yMax"] = nice_max_y
        # Add 1 to include starting point
        y_label_count = int((nice_max_y - nice_min_y) / nice_tick_y) + 1
        plot_obj["yLabels"] = [str(self.remove_exponent(p * nice_tick_y + nice_min_y)) for p in range(0, y_label_count)]
        
        # History plot (plot_time -> now)
        start_time = plot_time
        end_time = datetime.datetime.utcnow()
        x_labels = []
        x_labels_positions = []
        if is_forecast:
            # Forecast time labels (now -> plot_time)
            start_time, end_time = end_time, start_time
            current_time = start_time
            time_diff = end_time - start_time
            i = 0;
            while True:
                i += 1
                current_time = current_time.replace(day=current_time.day+1, second=0, hour=0, minute=0)
                if current_time >= end_time or i > 10:
                    break
                x_labels.append(current_time.strftime("%d.%m"))
                x_labels_positions.append(f"{(current_time - start_time) / time_diff:.3f}")
        else:
            time_labels_count = 4
            time_width_secs = (end_time - start_time).total_seconds()
            time_tick = time_width_secs / time_labels_count
            for i in [1, 2, 3]:
                x_labels.append((start_time + datetime.timedelta(seconds=time_tick * i)).strftime("%H:%M"))
                x_labels_positions.append(i * 0.25);

        plot_obj["xLabels"] = x_labels
        plot_obj["xLabelsPos"] = x_labels_positions
        return plot_obj


    def get_db_data(self):
        db_engine = db.create_engine(f"sqlite:///{os.getenv('HA_DB_PATH')}")
        db_connection = db_engine.connect()
        db_metadata = db.MetaData()
        states_table = db.Table('states', db_metadata, autoload=True, autoload_with=db_engine)
        query = db.select([states_table.columns.entity_id, states_table.columns.state, states_table.columns.last_updated]).where(db.and_(states_table.columns.domain == "sensor", states_table.columns.last_updated > self.filter_time)).order_by(db.asc(states_table.columns.last_updated))
        result_set = db_connection.execute(query).fetchall()
        return result_set


    def parse_db_data(self, result_set):
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
                    "y": list(),
                    "parsed": False
                }
            sensor_dict[sensor_name]["x"].append(sensor_state_date)
            sensor_dict[sensor_name]["y"].append(sensor_state)
        return sensor_dict
        

    def add_plots(self, data_json, sensor_dict):
        # Extract sensors mentioned in config
        for config_item in self.server.config_plots:
            entity_list = config_item["entities"]
            x_series = list()
            y_series = list()
            for entity in entity_list:
                sensor_name = "sensor." + entity
                if sensor_name not in sensor_dict:
                    print(f"ERROR SENSOR {sensor_name} NOT FOUND!")
                    continue
                sensor_obj = sensor_dict[sensor_name]
                if not sensor_obj["parsed"]:
                    # Normalize list to be from 0 to 1
                    sensor_obj['x'] = self.normalize_datetime_array(sensor_obj['x'])
                    # Remove dupliates
                    self.remove_duplicates(sensor_obj['y'], sensor_obj['x'])

                    sensor_obj["parsed"] = True
                x_series.append(sensor_obj['x'])
                y_series.append(sensor_obj['y'])
            if x_series and y_series:
                data_json["plots"].append(self.create_plot_obj(config_item["location"]
                                                               ,config_item["type"]
                                                               ,config_item["name"]
                                                               ,x_series
                                                               ,y_series
                                                               ,self.filter_time))


    def add_temp_sensors(self, data_json, sensor_dict):
        # Extract sensors mentioned in config
        for config_item in self.server.config_temp_sensors:
            sensor_obj = {
                "name": config_item["name"],
                "description": config_item["description"]
            }
            sensor_name_prefix = "sensor." + config_item["entity"]
            sensor_temp = sensor_name_prefix + "_temperature"
            sensor_humidity = sensor_name_prefix + "_humidity"
            if sensor_temp in sensor_dict:
                sensor_obj["temperature"] = sensor_dict[sensor_temp]['y'][-1]
            if sensor_humidity in sensor_dict:
                sensor_obj["humidity"] = sensor_dict[sensor_humidity]['y'][-1]
            data_json["sensors"].append(sensor_obj)

    
    def add_weather_data(self, data_json):
        start_time = time.time()
        # https://api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}&appid={API key}
        lat, lon = "53.13333", "23.16433"
        complete_url = "https://api.openweathermap.org/data/2.5/forecast?" + "appid=" + self.server.config_api_key + "&lat=" + lat + "&lon=" + lon + "&lang=pl&units=metric"
        print(f"Requesting forecast: {complete_url}")
        response = requests.get(complete_url)
        response_json = json.loads(response.text)
        # print(response_json)
        if response_json['cod'] != '200':
            print(f"Weather data failed. Time spent on request: {round(time.time() - start_time, 4)} secs")
            return
        weather_id_list = []
        weather_timestamp_list = []
        weather_y_list = []
        weather_rain_list = []
        for weather_json_obj in response_json['list']:
            weather_y_list.append(str(weather_json_obj['main']['temp']))
            weather_timestamp_list.append(datetime.datetime.utcfromtimestamp(weather_json_obj['dt']))
            weather_id_list.append(weather_json_obj['weather'][0]['id'])
            rain_value = 0
            if "rain" in weather_json_obj:
                rain_value = weather_json_obj["rain"]["3h"]
            weather_rain_list.append(rain_value)
        weather_timestamp_list = self.normalize_datetime_array(weather_timestamp_list)
        weather_obj = self.create_plot_obj("outside", "forecast", "", weather_timestamp_list, weather_y_list, datetime.datetime.utcfromtimestamp(response_json['list'][-1]['dt']))
        weather_obj['weather_id'] = weather_id_list
        rain_max_value = max(weather_rain_list)
        weather_obj["rain"] = weather_rain_list
        weather_obj["rainMax"] = rain_max_value
        # TODO fix xlabels
        data_json['weather'] = weather_obj
        print(f"Weather data prepared in: {round(time.time() - start_time, 4)} secs")


    def do_GET(self):
        url_parse_result = urlparse(self.path)
        if url_parse_result.path == "/refresh":
            print("Refreshing config...")
            self.server.read_config()
            return
        if url_parse_result.path == "/weather":
            test_dict = {}
            self.add_weather_data(test_dict)
            print(json.dumps(test_dict, indent=4))
            return
        start_time = time.time()
        self.send_response(200)
        self.end_headers()
        self.filter_time = datetime.datetime.utcnow() - datetime.timedelta(hours=self.server.config_home_plot_hours)
        data_json = {
            "time": datetime.datetime.now().strftime("%H:%M"),
            "plots": list(),
            "sensors": list()
        }
        # Query DB
        result_set = self.get_db_data()
        # Parase DB data
        sensor_dict = self.parse_db_data(result_set)
        # Add plots
        self.add_plots(data_json, sensor_dict)
        # Add latest sensor data
        self.add_temp_sensors(data_json, sensor_dict)
        # Add weather forecast
        self.add_weather_data(data_json)

        print(f"Request parsed in: {round(time.time() - start_time, 4)} secs")
        self.wfile.write(json.dumps(data_json).encode("utf-8"))


class DisplayServer:
    def __init__(self, port):
        ip = "0.0.0.0"
        if port == None:
            port = 8881
        print(f"Starting server at address: {ip}:{port}")
        self.httpd = HTTPServer((ip, port), DisplayServerHTTPRequestHandler)
        self.httpd.read_config = self.read_config
        self.httpd.read_config()
        self.httpd.serve_forever()

    def read_config(self):
        config = configparser.ConfigParser()
        config.read(os.getenv('SERVER_CONFIG_FILE'))
        self.httpd.config_api_key = config.get("Global", "api_key")
        self.httpd.config_plots = json.loads(config.get("Global", "plots"))
        self.httpd.config_home_plot_hours = int(config.get("Global", "home_plot_hours"))
        self.httpd.config_temp_sensors = json.loads(config.get("Global", "temp_sensors"))


if __name__ == '__main__':
    # Instantiate the parser
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('-p', type=int, help='Port')
    args = parser.parse_args()
    display_server = DisplayServer(args.p)
