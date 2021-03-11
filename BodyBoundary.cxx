
#include "BodyBoundary.hxx"

#include <functional>

#include <uf_defs.h>
#include <NXOpen/Part.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/Edge.hxx>
#include <NXOpen/Point.hxx>
#include <NXOpen/PointCollection.hxx>

using namespace NXOpen;
using namespace std;

BodyBoundary::BodyBoundary()
{
    head = nullptr;
}

BodyBoundary::~BodyBoundary()
{
    delete_tail(head);
}

PointListNode *BodyBoundary::find_end(PointListNode *node)
{
    if (node->next)
        return find_end(node->next);

    return node;
}

void BodyBoundary::delete_tail(PointListNode *node){
    if (node->next)
        delete_tail(node->next);

    delete node;
}

void BodyBoundary::add_point(Point *point)
{
    add_point(point->Coordinates());
}

void BodyBoundary::add_point(Point3d p)
{
    PointListNode *newNode = new PointListNode();
    newNode->point = p;
    newNode->next = nullptr;

    find_end(head)->next = newNode;
}

void BodyBoundary::get_points(Body *body)
{
    Point *p = nullptr;

    for (Edge *e: body->GetEdges())
    {
        try
        {
            // create center point on edge
            p = body->OwningPart()->Points()->CreatePoint(e, SmartObject::UpdateOptionWithinModeling);
            add_point(p->Coordinates());
        }

        // some points will error out, don't care
        catch (const exception &ex) {}
    }  
}

double BodyBoundary::minimum(Axis axis)
{
    struct PointListNode *node = head->next;
    double val = get_point_value(head, axis);

    while (node)
    {
        val = min(val, get_point_value(node, axis));

        node = node->next;
    }
    
    return val;
}

double BodyBoundary::maximum(Axis axis)
{
    struct PointListNode *node = head->next;
    double val = get_point_value(head, axis);

    while (node)
    {
        val = max(val, get_point_value(node, axis));

        node = node->next;
    }
    
    return val;
}

double BodyBoundary::get_point_value(PointListNode *pt, Axis axis)
{
    return get_point_value(pt->point, axis);
}

double BodyBoundary::get_point_value(Point3d pt, Axis axis)
{
    switch (axis)
    {
        case X:
            return pt.X;
            
        case Y:
            return pt.Y;
            
        case Z:
            return pt.Z;
        default:
            // UNREACHABLE
            return 0.0;
    }
}

double BodyBoundary::thickness()
{
    return maximum(Z) - minimum(Z);
}