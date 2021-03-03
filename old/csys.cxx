
#include <vector>
#include <sstream>

#include <uf_defs.h>
#include <NXOpen/NXException.hxx>
#include <NXOpen/Session.hxx>
#include <NXOpen/BasePart.hxx>
#include <NXOpen/CartesianCoordinateSystem.hxx>
#include <NXOpen/CoordinateSystemCollection.hxx>
#include <NXOpen/Features_DatumCsys.hxx>
#include <NXOpen/Features_DatumCsysBuilder.hxx>
#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/NXObject.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/PartCollection.hxx>
#include <NXOpen/Xform.hxx>
#include <NXOpen/XformCollection.hxx>

#include <NXOpen/LogFile.hxx>


using namespace NXOpen;
using namespace std;

void run(Session *nx_session)
{
    char *part_file_name = "C:\\Users\\PMiller1\\git\\nx-dxf\\1190181A_G1A-web_named_bodies.prt";

    /* Open part */
    PartLoadStatus *part_load_status;
    Part *part = dynamic_cast<Part *>(nx_session->Parts()->OpenActiveDisplay(part_file_name, DisplayPartOptionReplaceExisting, &part_load_status));
    delete part_load_status;
    
    nx_session->ApplicationSwitchImmediate("UG_APP_MODELING");

    set_wcs_to_face(part);

    nx_session->ApplicationSwitchImmediate("UG_APP_NOPART");
}

void set_wcs_to_face(Part *part)
{
    Features::Feature *nullNXOpen_Features_Feature(NULL);
    Features::DatumCsysBuilder *datum_csys_builder = part->Features()->CreateDatumCsysBuilder(nullNXOpen_Features_Feature);
    
    Point3d origin(0.0, 0.0, -0.5);
    Vector3d x_dir(1.0, 0.0, 0.0);
    Vector3d y_dir(0.0, 1.0, 0.0);
    Xform *xform = part->Xforms()->CreateXform(origin, x_dir, y_dir, SmartObject::UpdateOptionWithinModeling, 1.0);

    CartesianCoordinateSystem *ccord_sys = part->CoordinateSystems()->CreateCoordinateSystem(xform, SmartObject::UpdateOptionWithinModeling);

    /* create datum CSYS */
    datum_csys_builder->SetCsys(ccord_sys);
    NXObject *csys_commit = datum_csys_builder->Commit();

    Features::DatumCsys *datum_csys(dynamic_cast<NXOpen::Features::DatumCsys *>(csys_commit));
    datum_csys->SetWcsAtCsys();

    /* clean up pointers and builder */
    delete xform;
    delete ccord_sys;
    delete csys_commit;
    datum_csys_builder->Destroy();
}

extern "C" DllExport int ufusr_ask_unload()
{
    return (int)Session::LibraryUnloadOptionImmediately;
}

extern "C" DllExport void ufusr(char *param, int *retCode, int paramLen)
{
    Session *nx_session = Session::GetSession();

    nx_session->LogFile()->WriteLine("*************************************************************");
    nx_session->LogFile()->WriteLine("*                                                           *");
    nx_session->LogFile()->WriteLine("*                           Start                           *");
    nx_session->LogFile()->WriteLine("*                                                           *");
    nx_session->LogFile()->WriteLine("*************************************************************");
    

    run(nx_session);
    
    
    nx_session->LogFile()->WriteLine("*************************************************************");
    nx_session->LogFile()->WriteLine("*                                                           *");
    nx_session->LogFile()->WriteLine("*                            End                            *");
    nx_session->LogFile()->WriteLine("*                                                           *");
    nx_session->LogFile()->WriteLine("*************************************************************");

}
