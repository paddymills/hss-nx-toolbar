
import os
import logging

from datetime import date
import config


def init():
    user = os.getlogin()
    timestamp = date.today().isoformat()

    log_file = os.path.join( config.LOG_DIR, "{}_{}.log".format( user, timestamp ) )

    logging.basicConfig(
        filename=log_file,
        format='[%(asctime)s]%(levelname)s:%(messages)s',
        level=config.LOGGING_LEVEL
    )
