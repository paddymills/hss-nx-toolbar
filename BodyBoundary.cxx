
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
    if (head)
        delete_tail(head);
}

PointNode *BodyBoundary::find_end(PointNode *node)
{
    if (node->next)
        return find_end(node->next);

    return node;
}

void BodyBoundary::delete_tail(PointNode *node)
{
    if (node->next)
        delete_tail(node->next);

    delete node;
}

void BodyBoundary::add_point(Point *point)
{
    PointNode *newNode = new PointNode();
    
    newNode->point = point->Coordinates();
    newNode->next = nullptr;

    if (!head)
        head = newNode;
    else
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
            add_point(p);
        }

        // some points will error out, don't care
        catch (const exception &ex) {}
    }  
}

double BodyBoundary::minimum(Axis axis)
{
    struct PointNode *node = head->next;
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
    struct PointNode *node = head->next;
    double val = get_point_value(head, axis);

    while (node)
    {
        val = max(val, get_point_value(node, axis));

        node = node->next;
    }
    
    return val;
}

double BodyBoundary::get_point_value(PointNode *node, Axis axis)
{
    switch (axis)
    {
        case X:
            return node->point.X;
            
        case Y:
            return node->point.Y;
            
        case Z:
            return node->point.Z;
        default:
            // UNREACHABLE
            return 0.0;
    }
}

double BodyBoundary::thickness()
{
    return maximum(Z) - minimum(Z);
}