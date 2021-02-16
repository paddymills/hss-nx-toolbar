
#include <iostream>
#include <vector>
#include <string>

// NXOpen header files
#include <NXOpen/Session.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/PartCollection.hxx>
#include <NXOpen/PartLoadStatus.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/Sketch.hxx>
#include <NXOpen/SketchCollection.hxx>

// UG C++ header files
#include <ug_session.hxx>
#include <ug_part.hxx>
#include <ug_exception.hxx>

// UFunc Headers
#include <uf.h>
#include <uf_part.h>

using namespace NXOpen;
using namespace std;

void processBodies(Part*);
void processSketches(Part*);

int main(int argc, char* argv[])
{
    int errorCode = UF_initialize();
    if (errorCode != 0)
        cout << "Initialize result: " << errorCode << endl;

    /* Initialize the NX Open C++ API environment */
    Session *theSession = Session::GetSession();

    NXOpen::PartLoadStatus *loadStatus;

    /* Initialize the NX Open C++ API environment */
    char *filename = "1190181A_G1A-web.prt";
    Part *part = theSession->Parts()->Open(filename, &loadStatus);

    processBodies(part);
    processSketches(part);

    /* Terminate the API environment */
    errorCode = UF_terminate();

    return errorCode;
}

void processBodies(Part *part)
{
    BodyCollection *bodies = part->Bodies();
    BodyCollection::iterator iter;
    Body *body;
    
    cout << "\n----------\n  Bodies\n----------" << endl;;
    for (iter = bodies->begin(); iter != bodies->end(); iter++)
    {
        body = *iter;

        cout << "Body on layer ";
        cout << body->Layer();
        cout << endl;
    }
}

void processSketches(Part *part)
{
    SketchCollection *sketches = part->Sketches();
    SketchCollection::iterator iter;
    Sketch *sketch;

    cout << "\n------------\n  Sketches\n------------" << endl;
    for (iter = sketches->begin(); iter != sketches->end(); iter++)
    {
        sketch = *iter;

        NXString name = sketch->Name();

        if (name.GetText()[0] == 'Z')
        {
            cout << "Sketch: ";
            cout << name.GetText();
            cout << endl;
        }

    }
}