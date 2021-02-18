
#define stringify( name ) # name

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

// NXOpen header files
#include <NXOpen/Session.hxx>
#include <NXOpen/BasePart.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/PartCollection.hxx>
#include <NXOpen/PartLoadStatus.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/Sketch.hxx>
#include <NXOpen/SketchCollection.hxx>

#include <NXOpen/DexManager.hxx>
#include <NXOpen/LicenseManager.hxx>

// UFunc Headers
#include <uf.h>

using namespace NXOpen;
using namespace std;

void processBodies(Part*);
void processSketches(Part*);
void exportDxf(Part*, Session*);

int main(int argc, char* argv[])
{
    char *contextName = "DxfExport";

    /* Initialize the NX Open C++ API environment */
    Session *theSession = Session::GetSession();

    /* acquire license */
    theSession->LicenseManager()->Reserve("solid_modeling", contextName);

    /* Not used variables and enums */
    NXOpen::PartLoadStatus *loadStatus;
    BasePart::CloseWholeTree closeTree = BasePart::CloseWholeTreeTrue;
    BasePart::CloseModified closeModified = BasePart::CloseModifiedCloseModified;

    /* Initialize the NX Open C++ API environment */
    char *filename = "1190181A_G1A-web_named_bodies.prt";
    Part *part = theSession->Parts()->Open(filename, &loadStatus);

    cout << "Open Part: ";
    cout << part->Name().GetText();

    cout << " (";
    cout << part->GetStringAttribute("JobNo").GetText();
    cout << "_";
    cout << part->GetStringAttribute("Mark").GetText();
    cout << ")";
    cout << endl;

    processBodies(part);
    processSketches(part);
    exportDxf(part, theSession);

    /* Close the part */
    part->Close(closeTree, closeModified, nullptr);
    
    /* release license */
    theSession->LicenseManager()->ReleaseAll(contextName);

    return 0;
}

void processBodies(Part *part)
{
    BodyCollection *bodies = part->Bodies();
    BodyCollection::iterator iter;
    Body *body;
    
    cout << endl;
    cout << "  ----------" << endl;
    cout << "    Bodies  " << endl;
    cout << "  ----------" << endl;
    cout << endl;

    for (iter = bodies->begin(); iter != bodies->end(); iter++)
    {
        body = *iter;

        cout << "\t";
        cout << body->Name().GetText();
        cout << endl;
    }
}

void processSketches(Part *part)
{
    SketchCollection *sketches = part->Sketches();
    SketchCollection::iterator iter;
    Sketch *sketch;

    cout << endl;
    cout << "  ------------" << endl;
    cout << "    Sketches  " << endl;
    cout << "  ------------" << endl;
    cout << endl;

    for (iter = sketches->begin(); iter != sketches->end(); iter++)
    {
        sketch = *iter;

        const char *name = sketch->Name().GetText();

        if (strstr(name, "ZINC"))
            cout << "\t" << name << endl;

    }
}

void exportDxf(Part *part, Session *session)
{
    DxfdwgCreator *dxfCreator = session->DexManager()->CreateDxfdwgCreator();
    NXObject *result;
    
    // build file name
    ostringstream oss;
    oss << "C:\\Users\\PMiller1\\git\\nx-dxf\\output\\";
    oss << part->GetStringAttribute("JobNo").GetText();
    oss << "_";
    oss << part->GetStringAttribute("Mark").GetText();
    oss << ".dxf";


    /* set DXF exporter options */
    dxfCreator->SetSettingsFile("C:\\Users\\PMiller1\\git\\nx-dxf\\config\\export.def");
    dxfCreator->SetFileSaveFlag(false);
    dxfCreator->SetExportDestination(dxfCreator->ExportDestinationOptionNativeFileSystem);

    // set export location
    dxfCreator->SetOutputFile(oss.str());
    dxfCreator->SetOutputFileExtension(".dxf");

    // generate file
    cout << "Generating file: " << dxfCreator->OutputFile().GetText() << endl;
    result = dxfCreator->Commit();
}