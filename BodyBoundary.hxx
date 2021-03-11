
#ifndef BODY_BOUNDARY_H
#define BODY_BOUNDARY_H

#include <uf_defs.h>
#include <NXOpen/Body.hxx>
#include <NXOpen/Point.hxx>

using namespace NXOpen;
using namespace std;

struct PointListNode
{
  Point3d point;
  struct PointListNode *next;
};

class BodyBoundary
{
    private:

        PointListNode *head;

        PointListNode *find_end(PointListNode*);
        void delete_tail(PointListNode*);

        
    public:

        enum Axis{X, Y, Z};

        BodyBoundary();
        ~BodyBoundary();

        void add_point(Point*);
        void add_point(Point3d);
        void get_points(Body*);

        double minimum(Axis);
        double maximum(Axis);
        double thickness();
        double coverage(BodyBoundary*);

        double get_point_value(PointListNode*, Axis);
        double get_point_value(Point3d, Axis);
};



#endif BODY_BOUNDARY_H