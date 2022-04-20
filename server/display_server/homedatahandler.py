# plot parsing
# weather get (plot)
# db get (plots + sensors data)
import datetime
import re
import sqlalchemy as db
import os
import utility
from datahandler import DataHandler

class HomeDataHandler(DataHandler):
    def __init__(self, plot_hours, plot_list, sensor_list):
        super(HomeDataHandler, self).__init__(datetime.datetime.utcnow() - datetime.timedelta(hours=plot_hours))
        self.sensor_list = sensor_list
        self.plot_list = plot_list


    def plot_location(self):
        return "home"

    def plot_type(self):
        return "plot"

    def _calculate_plot_labels(self, plot_date):
        # History plot (plot_time -> now)
        x_labels = []
        x_labels_positions = []
        start_time = plot_date
        end_time = datetime.datetime.utcnow()
        time_labels_count = 4
        time_width_secs = (end_time - start_time).total_seconds()
        time_tick = time_width_secs / time_labels_count
        for i in [1, 2, 3]:
            x_labels.append((start_time + datetime.timedelta(seconds=time_tick * i)).strftime("%H:%M"))
            x_labels_positions.append(i * 0.25)
        return x_labels, x_labels_positions

    def add_data(self, data_json):
        # Query DB
        result_set = self._get_db_data()
        # Parase DB data
        sensor_dict = self._parse_db_data(result_set)
        # Add plots
        self._add_temp_sensor_plots(data_json, sensor_dict)
        # Add latest sensor data
        self._add_sensors(data_json, sensor_dict)


    def _get_db_data(self):
        db_engine = db.create_engine(f"sqlite:///{os.getenv('HA_DB_PATH')}")
        db_connection = db_engine.connect()
        db_metadata = db.MetaData()
        states_table = db.Table('states', db_metadata, autoload=True, autoload_with=db_engine)
        query = db.select([states_table.columns.entity_id, states_table.columns.state, states_table.columns.last_updated]).where(db.and_(states_table.columns.domain == "sensor", states_table.columns.last_updated > self.plot_data_time)).order_by(db.asc(states_table.columns.last_updated))
        result_set = db_connection.execute(query).fetchall()
        return result_set


    def _parse_db_data(self, result_set):
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

    def _add_sensors(self, data_json, sensor_dict):
        # Extract sensors mentioned in config
        for sensor_item in self.sensor_list:
            sensor_obj = {
                "name": sensor_item["name"],
                "description": sensor_item["description"]
            }
            sensor_name_prefix = "sensor." + sensor_item["entity"]
            sensor_temp = sensor_name_prefix + "_temperature"
            sensor_humidity = sensor_name_prefix + "_humidity"
            if sensor_temp in sensor_dict:
                sensor_obj["temperature"] = sensor_dict[sensor_temp]['y'][-1]
            if sensor_humidity in sensor_dict:
                sensor_obj["humidity"] = sensor_dict[sensor_humidity]['y'][-1]
            data_json["sensors"].append(sensor_obj)


    def _add_temp_sensor_plots(self, data_json, sensor_dict):
        # Extract sensors mentioned in config
        for plot_item in self.plot_list:
            entity_list = plot_item["entities"]
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
                    sensor_obj['x'] = utility.normalize_datetime_array(sensor_obj['x'])
                    # Remove dupliates
                    utility.remove_duplicates(sensor_obj['y'], sensor_obj['x'])
                    sensor_obj["parsed"] = True
                x_series.append(sensor_obj['x'])
                y_series.append(sensor_obj['y'])
            if x_series and y_series:
                data_json["plots"].append(self.create_plot_obj(plot_item["name"] ,x_series ,y_series))