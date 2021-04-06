
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#define _CRT_SECURE_NO_WARNINGS 1

#include "BodyBoundary.hxx"
#include "DxfNames.hxx"
#include "HssDxfDriver.hxx"
#include "HssDxfDriverUtils.hxx"

#include <experimental/filesystem>
#include <iomanip>

#include <uf_defs.h>
#include <NXOpen/Session.hxx>
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

namespace fs = experimental::filesystem;


extern "C" DllExport int ufusr_ask_unload()
{
    return (int)Session::LibraryUnloadOptionImmediately;
}

extern "C" DllExport void ufusr(char *param, int *retCode, int paramLen)
{
    HssDxfDriver driver;

    driver.test();
}

void HssDxfDriver::test()
{
    string PATH = "C:\\Users\\PMiller1\\git\\nx-dxf\\test_files\\";
    map<string, int> part_files{
        { "1190181A_G1A-web_named.prt", 0},
        { "1190181A_G2A-web.prt",       0},
        { "1190259A_m3g.prt",           1},
        { "1190259A_SP1-b.prt",         2},
        { "1190259A_SP2-c.prt",         2},
        { "1190259A_x1b.prt",           1}
    };

    for ( pair<string, int> t : part_files )
    {
        switch ( t.second )
        {
            case 0: // skip processing
                continue;
            
            case 1: // process, but don't export
                dry_run = true;
                break;
            
            case 2: // process and export
                dry_run = false;
                break;
            
            default: break;
        }

        try
        {
            process_part( (PATH + t.first).c_str() );
        }

        catch(const exception &ex)
        {
            log << "Exception caught: " << ex.what() << endl;
        }
    }
}

HssDxfDriver::HssDxfDriver()
{
    session = Session::GetSession();
    dry_run = false;
}

void HssDxfDriver::process_open_parts()
{
    for ( BasePart* part : *( session->Parts() ) )
        process_part( part );
}

void HssDxfDriver::process_parts(vector<const char*> part_files)
{
    PartLoadStatus *_ploadstat;
    DisplayPartOption _opt = DisplayPartOption::DisplayPartOptionReplaceExisting;
    BasePart* open_part;
    
    for ( const char* part_file : part_files )
    {
        // open part
        open_part = session->Parts()->OpenActiveDisplay(part_file, _opt, &_ploadstat);

        // process part
        process_part( part_file );

        // close part
        open_part->Close(BasePart::CloseWholeTreeTrue, BasePart::CloseModifiedCloseModified, nullptr);
    }

    // clean up objects
    delete _ploadstat;
    delete open_part;
}

void HssDxfDriver::process_part(const char* part_file)
{
    vector<const char*> vec;

    vec.push_back(part_file);

    process_parts( vec );
}

void HssDxfDriver::process_part(Part* part)
{
    // create dxf exporter
    //  this is per part, not per session
    export_worker.init_factory();

    // save initial part state to revert to at end
    // for some reason, certain parts throw exceptions to being closed with modifcations
    Session::UndoMarkId initial_state = session->SetUndoMark(Session::MarkVisibilityVisible, "dxf_initial");

    // enter modeling
    session->ApplicationSwitchImmediate("UG_APP_MODELING");

    // Orient View->Top
    ModelingView *modeling_view(dynamic_cast<ModelingView *>(part->ModelingViews()->FindObject("Trimetric")));
    modeling_view->Orient(View::CannedTop, View::ScaleAdjustmentFit);
    delete modeling_view;


    // log file name
    // ----------------------------------------
    //     Processing file: {filename}
    // ----------------------------------------
    fs::path *part_path = new fs::path(part->FullPath().GetText());
    string filename(part_path->filename().stem().string());
    log << endl;
    log << setfill('-');
    log << setw(filename.size() + 28) << "-" << endl;
    log << "\tProcessing file: " << filename << endl;
    log << setw(filename.size() + 28) << "-" << endl;
    log << setfill(' ');

    // enter modeling
    session->ApplicationSwitchImmediate("UG_APP_MODELING");

    // process part
    log.increase_indent();
    handle_part_properties(part);
    handle_sketches(part);
    handle_bodies(part);

    // reset for next part processing
    export_worker.rem_factory();
    annotations.clear();
    log.reset_indent();

    // revert part to initial state
    session->UndoToMark(initial_state, "dxf_initial");
}

void HssDxfDriver::handle_part_properties(Part* part)
{
    string text;

    // drawing
    text = get_part_property(part, "DWG_NUMBER");
    if ( !is_empty_property(text) )
        annotations["DRAWING"] = text;
}

void HssDxfDriver::handle_sketches(Part* part)
{
    log << endl;
    log << "+  Adding Sketches" << endl;
    log << "  -----------------" << endl;
    log.increase_indent();

    // Add ZINC sketches
    for (Sketch *sketch: *(part->Sketches()))
    {
        // skip if blacklisted
        if ( blacklist(sketch) )
        {
            log << "- Skipping (blacklisted): ";
            log << sketch->Name() << endl;
            continue;
        }

        // sketch is hidden: skip
        if ( sketch->IsBlanked() )
        {
            log << "- Skipping (blanked): ";
            log << sketch->Name() << endl;
            continue;
        }

        // sketch is visible: add to file
        log << "+ Adding: ";
        log << sketch->Name() << " -> ";

        // add sketch lines to sketches
        set_layer(sketch);
        if ( export_worker.add_sketch(sketch) )
            log << "OK" << endl;
        else
            log << "FAILED" << endl;
    }

    log.decrease_indent();
}

void HssDxfDriver::handle_bodies(Part* part)
{
    log << endl;
    log << "+ Adding Bodies" << endl;
    log << " ---------------" << endl;
    log.increase_indent();

    string body_name, export_file;
    map<Body*, string> body_names = get_export_names(part);
    bool export_result = true;

    // Add body to dxf export 
    for ( Body *body: *( part->Bodies() ) )
    {
        // skip if blacklisted
        if ( blacklist(body) )
        {
            log << "- Skipping blacklisted body: ";
            log << body->JournalIdentifier() << endl;
            continue;
        }

        // add body to export
        body->SetLayer(1);

        // body and export names
        export_file = DXF_OUTPUT_DIR;
        export_file.append( body_names[ body ] );
        export_file.append(".dxf");

        // handle body attributes (i.e. thickness)
        handle_thickness(body);
        
        // export body
        if ( !dry_run )
            export_result = export_worker.export_body( body, export_file.c_str() );

        // log success/failure
        if ( export_result )    log << "+ Exported body: ";
        else                    log << "* Error processing body: ";
        log << body_names[ body ] << endl;
    }

    log.decrease_indent();
}

void HssDxfDriver::handle_thickness(Body *body)
{
    NXObject *note;
    string note_text;
    double x, y;

    BodyBoundary *bound = new BodyBoundary(body);

    // add thickness annotation if minimum Z is not on XY plane
    if ( bound->minimum(BodyBoundary::Z) != 0.0 )
        annotations["THICKNESS"] = to_string(bound->thickness());

    // else -> remove thickness annotation
    else
        annotations.erase("THICKNESS");

    // add annotations, if any
    if ( annotations.size() > 0 )
    {
        x = bound->minimum(BodyBoundary::X) + NOTE_OFFSET;
        y = bound->minimum(BodyBoundary::Y) - NOTE_OFFSET;

        // set work layer for note to be created on
        body->OwningPart()->Layers()->SetWorkLayer(4);

        note = export_worker.add_annotations(annotations, x, y);
        log << "+ Annotations added" << endl;
    }
}
