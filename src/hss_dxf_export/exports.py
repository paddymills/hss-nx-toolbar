
from datetime import datetime
from config import config
from tracing import debug

class BodyExport:
    def __init__(self, body, sketches, name=None, **anno):
        self.body = body
        self.sketches = sketches
        self._name = name
        self.anno = anno

    def set_property(self, name, value):
        self.anno[name] = value

    @property
    def name(self):
        if self._name:
            return self._name
        
        raise Exception("BodyExport has no name assigned")

    @name.setter
    def name(self, value):
        debug("Setting BodyExport name to '{}'".format(value))
        self._name = value
    
    @property
    def annotation_text(self):
        timestamp = datetime.now().strftime(config.notes.datetime_format)
        rows = [
            ("EXPORTED", timestamp),
            ("THICKNESS", self.thickness),
            *self.anno.items()
        ]
    
        return [ "{}: {}".format(k, v) for k, v in rows ]

    @property
    def annotation_loc(self):
        x = y = float('inf')

        # get the minimum point of the body
        for edge in self.body.GetEdges():
            for vert in edge.GetVertices():
                x = min(x, vert.X)
                y = min(y, vert.Y)

        offset = config.notes.offset

        return (x + offset, y - offset, 0.0)
    
    @property
    def annotation_size(self):
        x_min = float('inf')
        x_max = float('-inf')

        # get the x maximum and minimum points of the body
        for edge in self.body.GetEdges():
            for vert in edge.GetVertices():
                x_min = min(x_min, vert.X)
                x_max = max(x_max, vert.X)

        multiplier = config.notes.size_multiplier
        size = (x_max - x_min) * multiplier
        debug("Annotation size for body '{}': {}".format(self.name, size))

        return size

    @property
    def thickness(self):
        z_min = float('-inf')
        z_max = float('inf')

        # get the z maximum and minimum points of the body
        for edge in self.body.GetEdges():
            for vert in edge.GetVertices():
                z_min = min(z_min, vert.Z)
                z_max = max(z_max, vert.Z)

        debug("Thickness for body '{}': {}".format(self.name, z_max - z_min))

        return round(z_max - z_min, 4)
