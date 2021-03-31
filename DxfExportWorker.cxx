
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#define _CRT_SECURE_NO_WARNINGS 1

#include "DxfExportWorker.hxx"
#include "BodyBoundary.hxx"
#include "HssDriverUtils.hxx"

#include <experimental/filesystem>
#include <map>
#include <iomanip>
#include <ctime>

#include <uf.h>
#include <uf_defs.h>
#include <NXOpen/Session.hxx>
#include <NXOpen/BasePart.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/PartCloseResponses.hxx>
#include <NXOpen/PartCollection.hxx>
#include <NXOpen/PartLoadStatus.hxx>
#include <NXOpen/DexBuilder.hxx>
#include <NXOpen/DexManager.hxx>
#include <NXOpen/DxfdwgCreator.hxx>

#include <NXOpen/Features_Feature.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/Edge.hxx>
#include <NXOpen/Face.hxx>
#include <NXOpen/Point.hxx>
#include <NXOpen/PointCollection.hxx>
#include <NXOpen/Sketch.hxx>
#include <NXOpen/SketchCollection.hxx>
#include <NXOpen/View.hxx>
#include <NXOpen/ViewCollection.hxx>

#include <NXOpen/DraftingManager.hxx>
#include <NXOpen/SelectDisplayableObject.hxx>
#include <NXOpen/Layer_LayerManager.hxx>
#include <NXOpen/Annotations_Annotation.hxx>
#include <NXOpen/Annotations_AnnotationManager.hxx>
#include <NXOpen/Annotations_LeaderBuilder.hxx>
#include <NXOpen/Annotations_LeaderData.hxx>
#include <NXOpen/Annotations_LeaderDataList.hxx>
#include <NXOpen/Annotations_OriginBuilder.hxx>

#include <NXOpen/LogFile.hxx>
#include <NXOpen/LicenseManager.hxx>
#include <NXOpen/NXObject.hxx>
#include <NXOpen/NXException.hxx>
#include <NXOpen/SelectNXObjectList.hxx>
#include <NXOpen/ModelingView.hxx>
#include <NXOpen/ModelingViewCollection.hxx>

using namespace NXOpen;
using namespace std;

namespace fs = experimental::filesystem;

// Initialize static variables
Session *(DxfExportWorker::nx_session) = NULL;
LogFile *(DxfExportWorker::nx_system_log) = NULL;

DxfExportWorker::DxfExportWorker()
{
    string log_filename = string(LOG_DIR).append("test.log");
    log.open(log_filename);

    try
    {
        char time_str[80];
        time_t raw_time;
        struct tm *time_info;

        time(&raw_time);
        time_info = localtime(&raw_time);
        strftime(time_str, 80, "(%d%b%Y-%I%M%S %P)", time_info);
        log << raw_time << "_" << getenv("USERNAME") << time_str << endl;
    }
    catch( const exception &ex ) {}
    

    log << endl;
    log << "****************************************************************" << endl;
    log << "*                                                              *" << endl;
    log << "*                    Begin file processing                     *" << endl;
    log << "*                                                              *" << endl;
    log << "****************************************************************" << endl;

    // init class members
    nx_session = Session::GetSession();
    nx_system_log = nx_session->LogFile();

    // get solid_modeling license
    DxfExportWorker::nx_session->LicenseManager()->Reserve("solid_modeling", nullptr);
    
    part = nullptr;
    dxf_factory = nullptr;
    body_names = map<string, string>();
    body_index = 1;

    dry_run = false;
}

DxfExportWorker::~DxfExportWorker()
{
    // release solid_modeling license
    DxfExportWorker::nx_session->LicenseManager()->Release("solid_modeling", nullptr);
    
    // close factory, if not null
    if (dxf_factory)
    {
        dxf_factory->Destroy();
        delete dxf_factory;
    }

    log << endl;
    log << "****************************************************************" << endl;
    log << "*                                                              *" << endl;
    log << "*                 Files successfully processed                 *" << endl;
    log << "*                                                              *" << endl;
    log << "****************************************************************" << endl;

    log.close();
}

void DxfExportWorker::init_factory() {
    // init dxf/dwg exporter
    dxf_factory = nx_session->DexManager()->CreateDxfdwgCreator();

    dxf_factory->SetSettingsFile(DXF_EXPORT_CONFIG);
    dxf_factory->SetExportData(DxfdwgCreator::ExportDataOptionDrawing);
    dxf_factory->SetViewEditMode(true);
    dxf_factory->SetFlattenAssembly(true);
    dxf_factory->SetExportAs(DxfdwgCreator::ExportAsOptionThreeD);
    dxf_factory->ExportSelectionBlock()->SetSelectionScope(ObjectSelector::ScopeSelectedObjects);
    dxf_factory->SetExportFacesAs(DxfdwgCreator::ExportFacesAsOptionsPolylineMesh);
    dxf_factory->SetProcessHoldFlag(true);

    // set up dxf/dwg export for file
    dxf_factory->SetInputFile(part->FullPath().GetText());
}

void DxfExportWorker::process_part(const char *part_file_name)
{
    // Open part
    PartLoadStatus *part_load_status;
    part = dynamic_cast<Part *>(DxfExportWorker::nx_session->Parts()->OpenActiveDisplay(part_file_name, DisplayPartOptionReplaceExisting, &part_load_status));
    delete part_load_status;

    // save initial part state to revert to at end
    // for some reason, certain parts throw exceptions to being closed with modifcations
    Session::UndoMarkId initial_state = nx_session->SetUndoMark(Session::MarkVisibilityVisible, "dxf_initial");

    // enter modeling
    nx_session->ApplicationSwitchImmediate("UG_APP_MODELING");

    fs::path *part_path = new fs::path(part->FullPath().GetText());
    string filename(part_path->filename().stem().string());

    log << endl;
    log << "" << setfill('-') << setw(filename.size() + 28) << "-" << endl;
    log << "\tProcessing file: " << filename << endl;
    log << "" << setfill('-') << setw(filename.size() + 28) << "-" << endl;
    log << setfill(' ');

    process_part();

    // revert part to initial state
    nx_session->UndoToMark(initial_state, "dxf_initial");

    // close part
    part->Close(BasePart::CloseWholeTreeTrue, BasePart::CloseModifiedCloseModified, nullptr);
    nx_session->ApplicationSwitchImmediate("UG_APP_NOPART");

    // clean up part objects
    delete part;
}

void DxfExportWorker::process_part()
{
    // create dxf exporter
    //  this is per part, not per session
    init_factory();

    // Orient View->Top
    ModelingView *modelingView(dynamic_cast<ModelingView *>(part->ModelingViews()->FindObject("Trimetric")));
    modelingView->Orient(View::CannedTop, View::ScaleAdjustmentFit);

    // ********************
    // *     DO STUFF     *
    // ********************
    handle_part_properties();
    add_sketches();
    export_bodies(); 

    // reset dxf/dwg exporter object selection
    dxf_factory->Destroy();
    dxf_factory = nullptr;

    body_names.clear();
    body_index = 1;
}

void DxfExportWorker::handle_part_properties()
{
    string text;

    // drawing
    text = get_part_property("DWG_NUMBER");
    if ( !is_empty_property(text) )
        annotations["DRAWING"] = text;

}

void DxfExportWorker::add_sketches()
{
    log << endl;
    log << "\t+  Adding Sketches" << endl;
    log << "\t  -----------------" << endl;

    // Add ZINC sketches
    for (Sketch *sketch: *(part->Sketches()))
    {
        // skip if blacklisted
        if ( blacklist(sketch) )
        {
            log << "\t\t- Skipping blacklisted sketch: ";
            log << sketch->Name().GetText() << endl;
            continue;
        }

        // sketch is hidden: skip
        if ( sketch->IsBlanked() )
        {
            log << "\t\t- Skipping blanked sketch: ";
            log << sketch->Name().GetText() << endl;
            continue;
        }

        // sketch is visible: add to file
        log << "\t\t+ Adding sketch: ";
        log << sketch->Name().GetText();
        log << " -> ";

        // add sketch lines to sketches
        set_layer(sketch);
        if (add_object_to_export(sketch->GetAllGeometry()))
            log << "OK" << endl;
        else
            log << "FAILED" << endl;
    }

}

void DxfExportWorker::export_bodies()
{
    log << endl;
    log << "\t+ Adding Bodies" << endl;
    log << "\t ---------------" << endl;

    string body_name;

    // Add body to dxf export 
    for ( Body *body: *( part->Bodies() ) )
    {
        // skip if blacklisted
        if ( blacklist(body) )
        {
            log << "\t\t- Skipping blacklisted body: ";
            log << body->JournalIdentifier().GetText() << endl;
            continue;
        }

        body_name = get_export_name(body);

        // Set output file name
        dxf_factory->SetOutputFile(DXF_OUTPUT_DIR + body_name + ".dxf");

        // add body to export
        body->SetLayer(1);
        add_purgeable_object_to_export(body);

        // handle body attributes (i.e. thickness)
        try
        {
            handle_body(body);
            
            // generate DXF file
            if ( !DxfExportWorker::dry_run )
            {
                /* 
                    delete part file if it exists
                    this seems to speed up export compared to overwriting files
                    also, it keeps from accumulating *_bk.dxf files
                */
                if (fs::exists(fs::path(dxf_factory->OutputFile().GetText())))
                    remove(dxf_factory->OutputFile().GetText());

                NXObject *generate_result = dxf_factory->Commit();
            }
            log << "\t\t+ Exported body: " << body_name << endl;
        }
        catch (const exception &ex)
        {
            log << "\t\t* Error processing body: " << body_name << endl;
        }
        
        // delete added purgeable objects (body, annotations, etc)
        purge_objects();
    }
}

void DxfExportWorker::handle_body(Body *body)
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
        part->Layers()->SetWorkLayer(4);

        note = add_annotations(x, y);
        add_purgeable_object_to_export(note);
    }
}

NXObject *DxfExportWorker::add_annotations(double x_loc, double y_loc)
{
    part->Drafting()->EnterDraftingApplication();
    part->Views()->WorkView()->UpdateCustomSymbols();
    part->Drafting()->SetTemplateInstantiationIsComplete(true);
    
    // turn off drawing layout (allows drafting tools in modeling)
    part->Drafting()->SetDrawingLayout(false);
    
    Annotations::DraftingNoteBuilder *note_factory;
    Annotations::SimpleDraftingAid *drafting_aid(NULL);
    note_factory = part->Annotations()->CreateDraftingNoteBuilder(drafting_aid);
    
    note_factory->Origin()->SetAnchor(Annotations::OriginBuilder::AlignmentPositionTopLeft);

    vector<NXString> anno_strings(annotations.size());
    map<string, string>::iterator itr;
    for (itr = annotations.begin(); itr != annotations.end(); ++itr)
        anno_strings.push_back(itr->first + ": " + itr->second);

    note_factory->Text()->TextBlock()->SetText(anno_strings);
    note_factory->Origin()->Plane()->SetPlaneMethod(Annotations::PlaneBuilder::PlaneMethodTypeXyPlane);
    
    // set leader settings
    Annotations::LeaderData *leader_data = part->Annotations()->CreateLeaderData();
    leader_data->SetArrowhead(Annotations::LeaderData::ArrowheadTypeFilledArrow);
    leader_data->SetVerticalAttachment(Annotations::LeaderVerticalAttachmentCenter);
    leader_data->SetStubSide(Annotations::LeaderSideInferred);
    note_factory->Leader()->Leaders()->Append(leader_data);

    // text size and justification
    note_factory->Style()->LetteringStyle()->SetGeneralTextSize(NOTE_SIZE);
    note_factory->Style()->LetteringStyle()->SetHorizontalTextJustification(Annotations::TextJustificationLeft);
    
    Annotations::Annotation::AssociativeOriginData note_origin;
    Annotations::Annotation *note_annotation(NULL);
    View *note_view(NULL);
    Point *note_point(NULL);

    // set origin settings
    note_origin.OriginType = Annotations::AssociativeOriginTypeDrag;
    note_origin.View = note_view;
    note_origin.ViewOfGeometry = note_view;
    note_origin.PointOnGeometry = note_point;
    note_origin.VertAnnotation = note_annotation;
    note_origin.VertAlignmentPosition = Annotations::AlignmentPositionTopLeft;
    note_origin.HorizAnnotation = note_annotation;
    note_origin.HorizAlignmentPosition = Annotations::AlignmentPositionTopLeft;
    note_origin.AlignedAnnotation = note_annotation;
    note_origin.DimensionLine = 0;
    note_origin.AssociatedView = note_view;
    note_origin.AssociatedPoint = note_point;
    note_origin.OffsetAnnotation = note_annotation;
    note_origin.OffsetAlignmentPosition = Annotations::AlignmentPositionTopLeft;
    note_origin.XOffsetFactor = 0.0;
    note_origin.YOffsetFactor = 0.0;
    note_origin.StackAlignmentPosition = Annotations::StackAlignmentPositionAbove;

    note_factory->Origin()->SetAssociativeOrigin(note_origin);
    
    // set note location
    Point3d note_location(x_loc, y_loc, 0.0);
    note_factory->Origin()->Origin()->SetValue(NULL, note_view, note_location);

    // create note
    NXObject *commit_result = note_factory->Commit();
    note_factory->Destroy();
    
    // switch back to modeling
    part->Drafting()->ExitDraftingApplication();

    log << "\t\t+ Annotations added" << endl;

    return commit_result;
}

bool DxfExportWorker::add_object_to_export(vector<NXObject *> objects)
{
    return dxf_factory->ExportSelectionBlock()->SelectionComp()->Add(objects);
}

bool DxfExportWorker::add_object_to_export(NXObject *object)
{
    return dxf_factory->ExportSelectionBlock()->SelectionComp()->Add(object);
}

bool DxfExportWorker::add_purgeable_object_to_export(NXObject *object)
{
    purgeable_objects.push_back(object);

    return add_object_to_export(object);
}

void DxfExportWorker::purge_objects()
{
    for (NXObject *object : purgeable_objects)
        dxf_factory->ExportSelectionBlock()->SelectionComp()->Remove(object);
}

string DxfExportWorker::get_part_property(string property_name)
{
    NXObject::AttributeInformation info_query;
    
    try
    {
        info_query = part->GetUserAttribute(property_name, NXObject::AttributeTypeString, -1);
    }
    catch (const exception &ex)
    {
        return "";
    }

    return info_query.StringValue.GetText();
}

string DxfExportWorker::get_part_property(vector<string> property_names)
{
    string result;
    for (string property_name : property_names)
    {
        result = get_part_property(property_name);
        if ( !is_empty_property(result) )
            return result;
    }

    return "";
}

bool DxfExportWorker::is_empty_property(string &value)
{
    for (char &c: value)
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


string DxfExportWorker::get_export_name(Body *body)
{
    // TODO: determine body type (web, flange?, other?)
    int number_of_bodies = get_number_of_body_exports(part);

    // get filename (no directories or extensions)
    fs::path *part_path = new fs::path(part->FullPath().GetText());
    string part_filename(part_path->filename().stem().string());

    // build base part file name
    string part_name;
    part_name.append(get_part_property( vector<string> { "JobNo", "JOB_NUMBER" } ));
    part_name.append("_");
    part_name.append(get_part_property( vector<string> { "Mark", "PIECE_MARK" } ));

    // make sure part filename starts with build part_name
    // or part_name is more than just job_
    // could be stale data
    if ( startswith(part_name, part_filename) || part_name.length() < 10 )
        part_name = part_filename;

    /* 
        ****************************************************************
        *                       single body part                       *
        ****************************************************************
    */
    if ( number_of_bodies == 1 )
        return part_name;

    /* 
        ****************************************************************
        *                          named body                          *
        ****************************************************************
    */
    if ( strlen(body->Name().GetText()) > 0 )
        return part_name + "-" + body->Name().GetText();

    /* 
        ****************************************************************
        *            attempt to infer from body boundaries             *
        ****************************************************************
    */
    if ( body_names.empty() )
    {
        if ( part_filename.find("web") != string::npos )
            body_names = get_web_names(part);
    }

    try
    {
        if ( body_names.find( body->JournalIdentifier().GetText() ) != body_names.end() )
            return part_name + "-" + body_names[ body->JournalIdentifier().GetText() ];
    }
    catch (exception &e) {}

    /* 
        ****************************************************************
        *                 default: return as body index                *
        ****************************************************************
    */
    // cannot infer so return "_{index}" (i.e. "_1")
    return part_filename + "_" + to_string(body_index++);
}