
import logging
import os
import re

from properties import get_part_properties
import config

import NXOpen

WEB_REGEX = re.compile(r"[-_]web", re.IGNORECASE)

class ExportNamer:

    def __init__(self, part):
        self.logger = logging.getLogger(__name__)
        
        self.exported_bodies = dict()
        self.has_named_bodies = False
        self.unnamed_body_id = 1

        self.SKIP_BODY_STR = "SKIP_THIS_BODY"
        self.stop_exporting = False

        self.is_web = bool( WEB_REGEX.search(part.Leaf) )

        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        #               base export name
        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        # in case part name has additional information in it
        #       (i.e. '_web', '_PM')
        # build the part name from part properties
        #
        # falls back to part file name if JOB or MARK are not found

        props = get_part_properties(part, props=config.PART_NAME_PROPS)
        if props["JOB"] and props["MARK"]:
            self.base_name = "{JOB}_{MARK}".format(**props)
        else:
            self.base_name =  part.Leaf


    def add_export(self, body, pts):
        if body.Name:
            if not self.has_named_bodies:
                self.remove_exported_bodies
                self.has_named_bodies = True

            if body.Name == config.SINGLE_BODY_EXPORT_NAME:
                self.logger.debug("Encountered single body export name")
                self.remove_exported_bodies()

                self.stop_exporting = True

                return self.base_name 


            self.has_named_bodies = True
            export_name = "{}-{}".format(self.base_name, body.Name)

        else:
            if self.has_named_bodies:
                return self.SKIP_BODY_STR

            export_name = "{}_{}".format(self.base_name, self.unnamed_body_id)
            self.unnamed_body_id += 1

        self.exported_bodies[export_name] = pts

        return export_name


    def rename_files(self):

        if self.is_web:
            self.logger.info("Renaming web export(s)")

            num_child_bodies = len(self.exported_bodies) - 1

            index = 1
            for name1, body1 in self.exported_bodies.items():
                for name2, body2 in self.exported_bodies.items():
                    if name1 == name2:
                        continue

                    if body1.is_parent(body2):
                        index = 0

                    elif body1.min_x > body2.min_x:
                        index += 1

                if index == 0:  # parent body (i.e. W1W2W3)
                    rename_to = ("W{}" * num_child_bodies).format(*range(1, num_child_bodies+1))

                else:           # child body (i.e. W1)
                    rename_to = "W{}".format(index)
 
                self.rename( name1, "{}-{}".format(self.base_name, rename_to) )


        # single body was exported
        elif len(self.exported_bodies) == 1:
            self.logger.info("Renaming single body export")
            self.rename(exported_bodies.keys()[0], self.base_name)
            return

        # else -> leave parts with named with number suffixes
        #      -> programmer will handle naming


    def remove_exported_bodies(self):
        # remove all exported files
        for name in self.exported_bodies.keys():
            os.remove(name)

        self.exported_bodies = dict()

    def rename(self, from_name, to_name):
        from_fn = os.path.join(config.DXF_OUTPUT_DIR, from_name)
        to_fn = os.path.join(config.DXF_OUTPUT_DIR, to_name)

        os.rename(from_fn, to_fn)
