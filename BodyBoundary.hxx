
#ifndef BODY_BOUNDARY_H
#define BODY_BOUNDARY_H

#include <uf_defs.h>
#include <NXOpen/Body.hxx>
#include <NXOpen/Point.hxx>

using namespace NXOpen;
using namespace std;

struct PointNode
{
  Point3d point;
  struct PointNode *next;
};

class BodyBoundary
{
    private:

        PointNode *head;

        PointNode *find_end(PointNode*);
        void delete_tail(PointNode*);

        
    public:

        enum Axis{X, Y, Z};

        BodyBoundary();
        BodyBoundary(Body*);
        ~BodyBoundary();

        void add_point(Point*);
        void get_points(Body*);

        double minimum(Axis);
        double maximum(Axis);
        double thickness();
        double coverage(BodyBoundary*);

        double get_point_value(PointNode*, Axis);
};



#endif BODY_BOUNDARY_H