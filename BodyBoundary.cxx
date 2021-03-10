
#include "BodyBoundary.hxx"

#include <uf_defs.h>
#include <NXOpen/Part.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/Edge.hxx>
#include <NXOpen/Point.hxx>
#include <NXOpen/PointCollection.hxx>

using namespace NXOpen;
using namespace std;

Boundary::Boundary()
{
    points = new PointLinkedList();
}

Boundary::~Boundary()
{
    delete_tail(head);
}

PointListNode *PointLinkedList::find_end(PointListNode *node)
{
    if (node->next)
        return find_end(node->next);

    return node;
}

void Boundary::delete_tail(PointListNode *node){
    if (node->next)
        delete_tail(node->next);

    delete node;
}

void Boundary::add_point(Point *point)
{
    add_point(point->Coordinates());
}

void Boundary::add_point(Point3d p)
{
    PointListNode *newNode = new PointListNode();
    newNode->point = p;
    newNode->next = nullptr;

    if (!head) // head == nullptr
        head = newNode;
    else
        find_end(head)->next = newNode;
}

void get_points(Body *body)
{
    Point *p = nullptr;

    for (Edge *e: body->GetEdges())
    {
        try
        {
            // create center point on edge
            p = body->OwningPart()->Points()->CreatePoint(e, SmartObject::UpdateOptionWithinModeling);
            points.add_point(p->Coordinates());
        }

        // some points will error out, don't care
        catch (const exception &ex) {}
    }  
}

double min_max(MinMax min_max_func, Axis axis)
{
    // set min or max function
    function<double (double, double)> min_max_func;
    switch (min_max_func)
        case MIN:
            min_max_func = min;
        case MAX:
            min_max_func = max;
            break;

    PointListNode *node = points.head;
    double val = get_point_value(points.head, axis);

    while (node)
    {
        val = min_max_func(val, get_point_value(node, axis));

        node = node->next;
    }
    
    
    return val;
}

double min(Axis axis)
{
    return min_max(min, axis);
}

double max(Axis axis)
{
    return min_max(max, axis);
}

static double BodyBoundary::get_point_value(Point3d *pt, Axis axis)
{
    switch (axis)
    {
        case X:
            return pt.X;
            
        case Y:
            return pt.Y;
            
        case Z:
            return pt.Z;
            
    }
}

double thickness()
{
    return min_max(MAX, Z) = min_max(MIN, Z);
}