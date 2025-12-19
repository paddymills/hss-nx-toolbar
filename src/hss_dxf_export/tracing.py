
import json
import os
from urllib import request

import logging

import NXOpen


class NXLogger(logging.Handler):
    def __init__(self, level=logging.INFO):
        super().__init__(level)
        self.session = NXOpen.Session.GetSession()

    def emit(self, record):
        # log to NX log file
        self.session.LogFile.WriteLine(record.getMessage())

class JsonFormatter(logging.Formatter):
    def format(self, record):
        log_record = {
            "timestamp": self.formatTime(record, datefmt="%Y-%m-%dT%H:%M:%S%z"),
            "level": record.levelname,
            "message": record.getMessage(),
            "filename": record.filename,
            "lineno": record.lineno,
        }
        return json.dumps(log_record)

class HttpHandler(logging.Handler):
    def __init__(self, level=logging.INFO):
        super().__init__(level)
        self.setFormatter(JsonFormatter())

        self.session = NXOpen.Session.GetSession()
        self.http_log_failures = 0

        self.reset_context()

    def reset_context(self):
        self.context = dict(user=os.getenv("USERNAME") or "unknown")

    def set_context(self, key, value):
        self.context[key] = value

    def emit(self, record):
        # avoid slowing down logging
        # we will assume the server is down after 5 failures
        if self.http_log_failures > 5:
            return

        # log to http server
        try:
            data = json.loads(self.format(record))
        except json.JSONDecodeError:
            data = { "message": self.format(record) }

        body = {
            **data,
            **self.context,
        }
        try:
            req = request.Request(
                "http://localhost:3000/log",
                data=json.dumps(body).encode("utf-8"),
                headers={"Content-Type": "application/json"},
                method="POST",
            )
            request.urlopen(req, timeout=1)
        except:
            self.http_log_failures += 1


http_handler = HttpHandler(logging.DEBUG)
nx_handler = NXLogger(logging.DEBUG)

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)
logger.addHandler(http_handler)
logger.addHandler(nx_handler)

# logging functions for convenience
debug = logger.debug
info = logger.info
warning = logger.warning
error = logger.error
critical = logger.critical