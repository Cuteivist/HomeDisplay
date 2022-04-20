import datetime
import time
import requests
import json
import utility
from datahandler import DataHandler

class ForecastDataHandler(DataHandler):
    def __init__(self, api_key):
        super(ForecastDataHandler, self).__init__(datetime.datetime.utcnow())
        self.api_key = api_key

    def plot_location(self):
        return "outside"

    def plot_type(self):
        return "forecast"

    def _calculate_plot_labels(self, plot_date):
        x_labels = []
        x_labels_positions = []
        # Forecast time labels (now -> plot_time)
        start_time = datetime.datetime.utcnow()
        end_time = plot_date
        current_time = start_time
        time_diff = end_time - start_time
        i = 0
        while True:
            i += 1
            current_time = current_time.replace(day=current_time.day+1, second=0, hour=0, minute=0)
            if current_time >= end_time or i > 10:
                break
            x_labels.append(current_time.strftime(f"%d.%m ({utility.weekday_to_str(current_time.weekday())})"))
            x_labels_positions.append(f"{(current_time - start_time) / time_diff:.3f}")
        return x_labels, x_labels_positions

    def _update_min_y(self, min_y, max_y):
        # For forecast we want to show rain at the bottom, 
        # which require givime more space to not overlap bar chart with plot
        diff = max_y - min_y
        return min_y - diff / 2

    def add_weather_data(self, data_json):
        start_time = time.time()
        # https://api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}&appid={API key}
        lat, lon = "53.13333", "23.16433"
        complete_url = "https://api.openweathermap.org/data/2.5/forecast?" + "appid=" + self.api_key + "&lat=" + lat + "&lon=" + lon + "&lang=pl&units=metric"
        print(f"Requesting forecast: {complete_url}")
        response = requests.get(complete_url)
        response_json = json.loads(response.text)
        # print(response_json)
        if response_json['cod'] != '200':
            print(f"Weather data failed. Time spent on request: {round(time.time() - start_time, 4)} secs")
            return
        self.plot_data_time = datetime.datetime.utcfromtimestamp(response_json['list'][-1]['dt'])
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
        weather_timestamp_list = utility.normalize_datetime_array(weather_timestamp_list)
        weather_obj = self.create_plot_obj("", weather_timestamp_list, weather_y_list)
        weather_obj['weather_id'] = weather_id_list
        rain_max_value = max(weather_rain_list)
        weather_obj["rain"] = weather_rain_list
        weather_obj["rainMax"] = rain_max_value
        # TODO fix xlabels
        data_json['weather'] = weather_obj
        print(f"Weather data prepared in: {round(time.time() - start_time, 4)} secs")