
import os
import re

import NXOpen

# TODO: modularize from dxf_export
NAME_STRIP_PATTERNS = [
    re.compile("_mfg", re.IGNORECASE),
    re.compile("_copy", re.IGNORECASE),
]

class Program:

    def __init__(self, part, program, logger=None):
        self.part = part
        self.mfg_program = program

        self.posts = list()
        self.pdf_files = list()

        self.log = logger

    def add_post(self, post):
        self.posts.append(post)

    def post(self):
        for post in self.posts:
            posted_file = post.postprocess(self.part, self.mfg_program, self.name)
            self.pdf_files.append(
                os.path.join(os.path.dirname(posted_file), self.name + '.pdf')
            )

    @property
    def name(self):
        mark = self.part.Name.split("_", 1)[-1]

        for pattern in NAME_STRIP_PATTERNS:
            mark = pattern.sub( "", mark )

        if "-" in self.mfg_program.Name:
            return "-".join([mark, self.mfg_program.Name.split("-")[1]])
        
        return mark
