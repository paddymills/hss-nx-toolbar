
import hss

import logging
from os import path
from enum import Enum


class TestType(Enum):
    SKIP = 1
    DRY_RUN = 2
    EXPORT = 3


def test():
    processor = hss.PartProcessor()
    logger = logging.getLogger(__name__)

    test_files_dir = r"C:\Users\PMiller1\git\nx-dxf\test_files"
    test_files = {
        "1190181A_G1A-web_named.prt"    : TestType.DRY_RUN,
        "1190181A_G2A-web.prt"          : TestType.DRY_RUN,
        "1190259A_m3g.prt"              : TestType.DRY_RUN,
        "1190259A_SP1-b.prt"            : TestType.DRY_RUN,
        "1190259A_SP2-c.prt"            : TestType.DRY_RUN,
        "1190259A_x1b.prt"              : TestType.DRY_RUN,
    }

    for part, test in test_files.items():
        logger.debug("{} '{}'".format(test.name, part))

        if test is TestType.SKIP:
            continue

        if test is TestType.DRY_RUN:
            processor.dry_run = True

        elif test is TestType.EXPORT:
            processor.dry_run = False

        processor.process_part( path.join(test_files_dir, part) )