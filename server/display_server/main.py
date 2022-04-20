from urllib.parse import urlparse
import time
import argparse
import json
from http.server import HTTPServer, BaseHTTPRequestHandler
import configparser
import os
import datetime
# Project files
from homedatahandler import HomeDataHandler
from forecastdatahandler import ForecastDataHandler
from sleephandler import SleepHandler

class DisplayServerHTTPRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        url_parse_result = urlparse(self.path)
        if url_parse_result.path == "/refresh":
            print("Refreshing config...")
            self.server.read_config()
            return
        if url_parse_result.path == "/test":
            test_dict = {}
            self.add_weather_data(test_dict)
            self.wfile.write(json.dumps(test_dict).encode("utf-8"))
            return
        start_time = time.time()
        self.send_response(200)
        self.end_headers()
        data_json = {
            "time": datetime.datetime.now().strftime("%H:%M"),
            "plots": list(),
            "sensors": list()
        }
        # Add home sensors data
        homeDataHandler = HomeDataHandler(self.server.config_home_plot_hours, 
                                          self.server.config_plots,
                                          self.server.config_sensors)
        homeDataHandler.add_data(data_json)
        # Add weather forecast
        forecastDataHandler = ForecastDataHandler(self.server.config_api_key)
        forecastDataHandler.add_weather_data(data_json)
        # Add how long to sleep
        sleephandler = SleepHandler()
        sleephandler.add_sleep_info(data_json)
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
        self.httpd.config_sensors = json.loads(config.get("Global", "sensors"))


if __name__ == '__main__':
    # Instantiate the parser
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('-p', type=int, help='Port')
    args = parser.parse_args()
    display_server = DisplayServer(args.p)
