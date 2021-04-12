
#include "config.hxx"
#include "HssPartProcessing.hxx"
#include "HssUtils.hxx"
#include "NxDxfExport.hxx"
#include "NxUtils.hxx"

#include <iomanip>

#include <uf_defs.h>
#include <NXOpen/Session.hxx>
#include <NXOpen/LicenseManager.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/PartCollection.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/Sketch.hxx>
#include <NXOpen/SketchCollection.hxx>

#include <NXOpen/View.hxx>
#include <NXOpen/Layer_LayerManager.hxx>
#include <NXOpen/ModelingView.hxx>
#include <NXOpen/ModelingViewCollection.hxx>

using namespace NXOpen;
using namespace std;

void HssPartProcessing::process_parts(vector<string> part_files)
{
    // init class members
    PartLoadStatus* _ploadstat;
    DisplayPartOption _opt = DisplayPartOption::DisplayPartOptionReplaceExisting;
    BasePart* open_part;

    // get solid_modeling license
    session->LicenseManager()->Reserve("solid_modeling", nullptr);

    for ( string part_file : part_files )
    {
        log.reset_indent();
        
        // open part
        open_part = session->Parts()->OpenActiveDisplay(part_file, _opt, &_ploadstat);

        // process part
        process_part( open_part );

        // close part
        open_part->Close(BasePart::CloseWholeTreeTrue, BasePart::CloseModifiedCloseModified, nullptr);
        session->ApplicationSwitchImmediate("UG_APP_NOPART");
    }

    // clean up objects
    delete _ploadstat;
    delete open_part;

    // release solid_modeling license
    session->LicenseManager()->Release("solid_modeling", nullptr);
}

void HssPartProcessing::process_part(Part* part)
{
    // enter modeling
    session->ApplicationSwitchImmediate("UG_APP_MODELING");

    // create dxf exporter
    //  this is per part, not per session
    exp = new NxDxfExport();
    exp->set_input_file(part);

    // save initial part state to revert to at end
    // for some reason, certain parts throw exceptions to being closed with modifcations
    Session::UndoMarkId initial_state = session->SetUndoMark(Session::MarkVisibilityVisible, "dxf_initial");

    set_top_view(part);

    // log processing file name
    string _fn = filename( part->FullPath().GetText() );
    log << endl << setfill('-');
    log << setw(_fn.size() + 28) << "-" << endl;
    log << "\tProcessing file: " << _fn << endl;
    log << setw(_fn.size() + 28) << "-" << endl;
    log << setfill(' ');

    try { 
        log.increase_indent();

        // process part
        handle_properties(part);
        handle_sketches(part);
        handle_bodies(part);
    
        // revert part to initial state
        session->UndoToMark(initial_state, "dxf_initial");
    }
    catch ( const exception& ex ) {}

    delete exp;
}

void HssPartProcessing::handle_sketches(Part* part)
{
    for (Sketch* sketch: *(part->Sketches()))
    {
        if ( blacklist(sketch) )
            continue;

        exp->add_sketch(sketch);
        log << "+ [sketch] " << sketch << endl;
        
    }
}

void HssPartProcessing::handle_bodies(Part* part)
{
    string body_name, export_file;
    bool export_result = true;

    // Add body to dxf export 
    for ( Body* body : *( part->Bodies() ) )
    {
        if ( blacklist(body) )
            continue;

        if ( !dry_run )
            exp->export_body( body );

        log << "> [body] " << body << endl;
    }
}

void HssPartProcessing::handle_properties(Part* part)
{
    string text;

    for ( pair<string, string> it : config::PROPS )
    {
        text = get_part_property(part, it.first.c_str());
        if ( !is_empty_property(text) )
            log << "+ [property] " << it.second << ": " << text << endl;
    }
}
