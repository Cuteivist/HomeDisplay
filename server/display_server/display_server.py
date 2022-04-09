import json
import argparse
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
    def __init__(self, port):
        ip = "0.0.0.0"
        if port == None:
            port = 8881
        print(f"Starting server at address: {ip}:{port}")
        httpd = HTTPServer((ip, port), DisplayServerHTTPRequestHandler)
        httpd.serve_forever()


if __name__ == '__main__':
    # Instantiate the parser
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('-p', type=int, help='Port')
    args = parser.parse_args()
    display_server = DisplayServer(args.p)
