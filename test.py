from datetime import datetime
import json
import os
from urllib import request

import logging

import NXOpen

logging.basicConfig(level=logging.DEBUG)


class NXLogger(logging.Logger):
    def __init__(self, name, level=logging.INFO):
        super().__init__(name, level)
        self.session = NXOpen.Session.GetSession()

    def log(self, message):
        # log to NX log file
        self.session.LogFile.WriteLine(message)


class HttpHandler(logging.Handler):
    def __init__(self, level=logging.INFO):
        super().__init__(level)
        self.setFormatter(
            logging.Formatter(
                json.dumps(
                    {
                        "timestamp": "%(asctime)s",
                        "level": "%(levelname)s",
                        "message": "%(message)s",
                    }
                ),
                datefmt="%Y-%m-%dT%H:%M:%S%z",
            )
        )

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
        body = {
            **json.loads(self.format(record)),
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

l = logging.getLogger(__name__)
l.addHandler(http_handler)

s = NXOpen.Session.GetSession()

l.debug("============ Start =============")

wp = s.Parts.Work
http_handler.set_context("partFile", wp.FullPath)

l.warning(wp.Name)
l.debug("all attr")
for x in wp.GetUserAttributes(False):
    l.info("{}: {}".format(x.Title, x.StringValue))

http_handler.reset_context()
l.debug("============  End  =============")
