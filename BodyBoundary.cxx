
#include "BodyBoundary.hxx"

#include <functional>

#include <uf_defs.h>
#include <NXOpen/Part.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/Edge.hxx>
#include <NXOpen/Point.hxx>
#include <NXOpen/PointCollection.hxx>
#include <NXOpen/LogFile.hxx>

using namespace NXOpen;
using namespace std;

BodyBoundary::BodyBoundary()
{
    head = nullptr;
}

BodyBoundary::BodyBoundary(Body *body)
{
    head = nullptr;

    get_points(body);
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
    Point3d *coord = &(point->Coordinates());
    
    newNode->point = Point3d(coord->X, coord->Y, coord->Z);
    newNode->next = nullptr;

    if (!head)
        head = newNode;
    else
        find_end(head)->next = newNode;
}

void BodyBoundary::get_points(Body *body)
{
    Point *p = nullptr;

    // save initial part state to revert to at end
    Session *session = Session::GetSession();
    Session::UndoMarkId initial_state = session->SetUndoMark(Session::MarkVisibilityVisible, "get_point_coord");

    for (Edge *e: body->GetEdges())
    {
        try
        {
            // create center point on edge
            p = body->OwningPart()->Points()->CreatePoint(e, SmartObject::UpdateOptionWithinModeling);
            add_point(p);
        }

        // some points will error out, don't care
        catch (const exception &ex) {
            session->LogFile()->Write("\n");
        }
    }

    // revert part to initial state
    session->UndoToMark(initial_state, "get_point_coord");
}

double BodyBoundary::minimum(char axis)
{
    switch (axis)
    {
        case 'x':
        case 'X':
            return minimum(X);
        case 'y':
        case 'Y':
            return minimum(Y);
        case 'z':
        case 'Z':
            return minimum(Z);
        
        default:
            throw "invalid axis";
    }
}

double BodyBoundary::minimum(Axis axis)
{
    if ( !head )
        return 0.0;

    struct PointNode *node = head->next;
    double val = get_point_value(head, axis);

    while (node)
    {
        val = min(val, get_point_value(node, axis));

        node = node->next;
    }
    
    return val;
}

double BodyBoundary::maximum(char axis)
{
    switch (axis)
    {
        case 'x':
        case 'X':
            return maximum(X);
        case 'y':
        case 'Y':
            return maximum(Y);
        case 'z':
        case 'Z':
            return maximum(Z);
        
        default:
            throw "invalid axis";
    }
}

double BodyBoundary::maximum(Axis axis)
{
    if ( !head )
        return 0.0;

    struct PointNode *node = head->next;
    double val = get_point_value(head, axis);

    while (node)
    {
        val = max(val, get_point_value(node, axis));

        node = node->next;
    }
    
    return val;
}

double BodyBoundary::distance(Axis axis)
{
    return maximum(axis) - minimum(axis);
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

bool BodyBoundary::point_equals(PointNode *node1, PointNode *node2)
{
    return point_equals(node1->point, node2->point);
}

bool BodyBoundary::point_equals(PointNode *node1, Point3d point2)
{
    return point_equals(node1->point, point2);
}

bool BodyBoundary::point_equals(Point3d point1, Point3d point2)
{
    if (point1.X == point2.X)
        if (point1.Y == point2.Y)
            if (point1.Z == point2.Z)
                return true;

    return false;
}

bool BodyBoundary::point_exists(PointNode *node)
{
    return point_exists(node->point);
}

bool BodyBoundary::point_exists(Point3d point)
{
    PointNode *node = head;

    while (node)
    {
        if (point_equals(node->point, point))
            return true;

        node = node->next;
    }

    return false;
}

double BodyBoundary::thickness()
{
    return maximum(Z) - minimum(Z);
}

double BodyBoundary::coverage(BodyBoundary *other)
{
    int coincident_points = 0;
    int number_of_points = 0;

    PointNode *node = head;

    while(node)
    {
        if (other->point_exists(node->point))
            coincident_points++;

        number_of_points++;
        node = node->next;
    }

    return static_cast<double>(coincident_points) / number_of_points;
}