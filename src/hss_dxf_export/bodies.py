
import logging

import NXOpen


logger = logging.getLogger(__name__)

class Point:

    def __init__(self, x, y, z):
        self.X = x
        self.Y = y
        self.Z = z

    
    def __eq__(self, other):
        if self.X == other.X:
            if self.Y == other.Y:
                if self.Z == other.Z:
                    return True

        return False


class BodyBound:

    def __init__(self, body):
        self._points = list()

        for edge in body.GetEdges():
            for vert in edge.GetVertices():
                self.add_point(vert)


    def add_point(self, point):
        _x = round(point.X, 4)
        _y = round(point.Y, 4)
        _z = round(point.Z, 4)

        logger.trace("Adding point ({} , {}, {})".format(_x, _y, _z))

        self._points.append( Point(_x, _y, _z) )


    def has_point(self, point):
        for _pt in self._points:
            if _pt == point:
                return True

        return False

    
    def coverage(self, other):
        # returns the percentage of points in self's body
        #   that are also in other's body

        coincident_points = 0

        for point in self._points:
            if other.has_point(point):
                coincident_points += 1

        return coincident_points / len(self._points)

    
    def is_parent(self, other):
        self_cov = self.coverage(other)
        other_cov = other.coverage(self)

        # if self is parent of other
        #   - other will have more coincident points that this
        #   - other's coverage will be more than 50%
        #       (probably more like 75% or 90%, but 50% will work)
        #       (this filters out 2 bodies with a coincident edge such as a W1 and W2)

        if self_cov < other_cov and other_cov > 0.5:
            return True

        return False


    @property
    def min_x(self):
        return self._extrema("X", min)


    @property
    def min_y(self):
        return self._extrema("Y", min)


    @property
    def min_z(self):
        return self._extrema("Z", min)


    @property
    def max_x(self):
        return self._extrema("X", max)


    @property
    def max_y(self):
        return self._extrema("Y", max)


    @property
    def max_z(self):
        return self._extrema("Z", max)


    @property
    def len_x(self):
        return self._len("X")


    @property
    def len_y(self):
        return self._len("Y")


    @property
    def len_z(self):
        return self._len("Z")

    
    def _extrema(self, axis_attr, minmax_func):
        return minmax_func([ getattr(x, axis_attr) for x in self._points] )


    def _len(self, axis_attr):
        return self._extrema(axis_attr, max) - self._extrema(axis_attr, min)
