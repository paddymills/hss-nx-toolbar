
#include "NxUtils.hxx"

#include <uf_defs.h>
#include <NXOpen/Part.hxx>
#include <NXOpen/ModelingView.hxx>
#include <NXOpen/ModelingViewCollection.hxx>
#include <NXOpen/View.hxx>

using namespace NXOpen;
using namespace std;

string get_part_property(Part* part, const char* property_name)
{
    NXObject::AttributeInformation info_query;
    
    try
    {
        info_query = part->GetUserAttribute(property_name, NXObject::AttributeTypeString, -1);

        return info_query.StringValue.GetText();
    }

    // error -> return empty string
    catch (const exception& ex) { return ""; }
}

string get_part_property(Part* part, const vector<char*> property_names)
{
    string result;
    for (const char* property_name : property_names)
    {
        result = get_part_property(part, property_name);
        if ( !is_empty_property(result) )
            return result;
    }

    return "";
}

bool is_empty_property(string value)
{
    for (char& c: value)
    {
        switch (c)
        {
            case ' ':
            case 'X':
            case 'x':
                break;
            default:
                return false;
        }
    }

    return true;
}

void set_top_view(Part* part)
{
    // Orient View->Top
    ModelingView* modeling_view(dynamic_cast<ModelingView*>(part->ModelingViews()->FindObject("Trimetric")));
    modeling_view->Orient(View::CannedTop, View::ScaleAdjustmentFit);
    delete modeling_view;
}