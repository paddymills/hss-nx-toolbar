
#ifndef ANNOTATION_H
#define ANNOTATION_H

#include "config.hxx"
#include <vector>

#include <uf.h>
#include <NXOpen/Part.hxx>
#include <NXOpen/NXObject.hxx>
#include <NXOpen/DraftingManager.hxx>
#include <NXOpen/SelectDisplayableObject.hxx>
#include <NXOpen/Annotations_Annotation.hxx>
#include <NXOpen/Annotations_AnnotationManager.hxx>
#include <NXOpen/Annotations_LeaderBuilder.hxx>
#include <NXOpen/Annotations_LeaderData.hxx>
#include <NXOpen/Annotations_LeaderDataList.hxx>
#include <NXOpen/Annotations_OriginBuilder.hxx>

using namespace NXOpen;
using namespace std;

class HssAnnotation
{
    private:
        NXObject* _annotation;
        double x, y;
        vector<NXString> notes;

    public:
        HssAnnotation();
        ~HssAnnotation() { delete _annotation; }

        void set_x(double _x) { x = _x; }
        void set_y(double _y) { y = _y; }
        void set_xy(double _x, double _y) { x = _x; y = _y; }
        void add_notes(map<string, string>);
        void add_notes(vector<NXString>);

        NXObject* create(Part*);
        NXObject* revise(Part*);
};


#endif