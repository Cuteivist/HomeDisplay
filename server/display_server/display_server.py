import json
from http.server import HTTPServer, BaseHTTPRequestHandler
from sensors.globals import get_sensor_list


class DisplayServerHTTPRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.end_headers()
        # TODO parse data to send to lower display processing as much as possible
        data_json = { "test": True, "test2": 20}
        self.wfile.write(json.dumps(data_json).encode("utf-8"))


class DisplayServer:
    def __init__(self):
        self.sensor_list = get_sensor_list()
        # TODO add method to mock some data to sensors (change it over time)
        port = 8881
        ip = "pc.lan"  # TODO change to 'pi.lan' when moved to RPI
        httpd = HTTPServer((ip, port), DisplayServerHTTPRequestHandler)
        httpd.serve_forever()


if __name__ == '__main__':
    display_server = DisplayServer()
