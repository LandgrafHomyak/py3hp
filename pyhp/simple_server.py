import os
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

from .interpreter import interpret_as_subprocess


def serve_forever(root_path, port=8080, host="localhost"):
    root_path = os.path.abspath(root_path)

    class HttpProcessor(BaseHTTPRequestHandler):
        def do_GET(self):
            # self.send_response(200)
            if "?" in self.path:
                path, args = self.path.split("?")
            else:
                path = self.path
                args = ""

            try:
                if path.endswith(".py3hp"):
                    pg = interpret_as_subprocess(os.path.normpath(os.path.join(root_path, "." + path)), args)
                else:
                    with open(os.path.normpath(os.path.join(root_path, "." + path)), "rb") as fin:
                        pg = fin.read()

            except FileNotFoundError:
                self.send_response(404)
            else:
                self.send_response(200)
            # self.send_header('content-type', 'text/html')
            self.end_headers()
            self.wfile.write(pg)

    serv = ThreadingHTTPServer((host, port), HttpProcessor)
    serv.serve_forever()


if __name__ == "__main__":
    serve_forever("P:/Python-Hypertext-Preprocessor")
