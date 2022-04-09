import json
from http.server import HTTPServer, BaseHTTPRequestHandler
from datetime import datetime

class DisplayServerHTTPRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.end_headers()
        data_json = {
            "time": datetime.now().strftime("%H:%M:%S")
        }
        self.wfile.write(json.dumps(data_json).encode("utf-8"))


class DisplayServer:
    def __init__(self):
        port = 8881
        ip = "0.0.0.0"
        httpd = HTTPServer((ip, port), DisplayServerHTTPRequestHandler)
        httpd.serve_forever()


if __name__ == '__main__':
    display_server = DisplayServer()
