
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1

#include "DxfExportWorker.hxx"
#include "BodyBoundary.hxx"
#include "HssDxfDriverUtils.hxx"

#include <experimental/filesystem>
#include <map>

#include <uf.h>
#include <uf_defs.h>
#include <NXOpen/Session.hxx>
#include <NXOpen/LicenseManager.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/PartCollection.hxx>
#include <NXOpen/DexBuilder.hxx>
#include <NXOpen/DexManager.hxx>
#include <NXOpen/DxfdwgCreator.hxx>

#include <NXOpen/Body.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/Point.hxx>
#include <NXOpen/PointCollection.hxx>
#include <NXOpen/Sketch.hxx>
#include <NXOpen/SketchCollection.hxx>
#include <NXOpen/View.hxx>
#include <NXOpen/ViewCollection.hxx>

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

namespace fs = experimental::filesystem;

DxfExportWorker::DxfExportWorker()
{
    // init class members
    session = Session::GetSession();

    // get solid_modeling license
    session->LicenseManager()->Reserve("solid_modeling", nullptr);
    
    dxf_factory = nullptr;
    anno_x = 0.0 + NOTE_OFFSET;
    anno_y = 0.0 - NOTE_OFFSET;
}

DxfExportWorker::~DxfExportWorker()
{
    // release solid_modeling license
    session->LicenseManager()->Release("solid_modeling", nullptr);
    
    // close factory, if not null
    if (dxf_factory)
        rem_factory();
}

void DxfExportWorker::init_factory()
{
    // init dxf/dwg exporter
    dxf_factory = session->DexManager()->CreateDxfdwgCreator();

    dxf_factory->SetSettingsFile(DXF_EXPORT_CONFIG);
    dxf_factory->SetExportData(DxfdwgCreator::ExportDataOptionDrawing);
    dxf_factory->SetViewEditMode(true);
    dxf_factory->SetFlattenAssembly(true);
    dxf_factory->SetExportAs(DxfdwgCreator::ExportAsOptionThreeD);
    dxf_factory->ExportSelectionBlock()->SetSelectionScope(ObjectSelector::ScopeSelectedObjects);
    dxf_factory->SetExportFacesAs(DxfdwgCreator::ExportFacesAsOptionsPolylineMesh);
    dxf_factory->SetProcessHoldFlag(true);
}

bool DxfExportWorker::export_body(Body* body)
{
    try
    {
        // add body to export
        body->SetLayer(PROFILE_LAYER);
        add_purgeable_object_to_export(body);

        /* 
            delete part file if it exists
            this seems to speed up export compared to overwriting files
            also, it keeps from accumulating* _bk.dxf files
        */
        if ( fs::exists( fs::path( dxf_factory->OutputFile().GetText() ) ) )
            remove( dxf_factory->OutputFile().GetText() );

        NXObject* generate_result = dxf_factory->Commit();
        
        // delete added purgeable objects (body, annotations, etc)
        purge_objects();

        return true;
    }
    catch (const exception& ex) { return false; }
}

NXObject* DxfExportWorker::add_annotations(map<string, string> anno_map, double x_loc, double y_loc)
{
    set_annotation_xy(x_loc, y_loc);

    return add_annotations(anno_map);
}

NXObject* DxfExportWorker::add_annotations(map<string, string> anno_map)
{
    vector<NXString> anno_strings(anno_map.size());

    map<string, string>::iterator itr;
    for (itr = anno_map.begin(); itr != anno_map.end(); ++itr)
        anno_strings.push_back(itr->first + ": " + itr->second);

    return add_annotations(anno_strings);
}

NXObject* DxfExportWorker::add_annotations(vector<NXString> anno_strings, double x_loc, double y_loc)
{
    set_annotation_xy(x_loc, y_loc);

    return add_annotations(anno_strings);
}

NXObject* DxfExportWorker::add_annotations(vector<NXString> anno_strings)
{
    Part* part = session->Parts()->Display();

    part->Drafting()->EnterDraftingApplication();
    part->Views()->WorkView()->UpdateCustomSymbols();
    part->Drafting()->SetTemplateInstantiationIsComplete(true);
    
    // turn off drawing layout (allows drafting tools in modeling)
    part->Drafting()->SetDrawingLayout(false);
    
    Annotations::DraftingNoteBuilder* note_factory;
    Annotations::SimpleDraftingAid* drafting_aid(NULL);
    note_factory = part->Annotations()->CreateDraftingNoteBuilder(drafting_aid);
    
    note_factory->Origin()->SetAnchor(Annotations::OriginBuilder::AlignmentPositionTopLeft);

    note_factory->Text()->TextBlock()->SetText(anno_strings);
    note_factory->Origin()->Plane()->SetPlaneMethod(Annotations::PlaneBuilder::PlaneMethodTypeXyPlane);
    
    // set leader settings
    Annotations::LeaderData* leader_data = part->Annotations()->CreateLeaderData();
    leader_data->SetArrowhead(Annotations::LeaderData::ArrowheadTypeFilledArrow);
    leader_data->SetVerticalAttachment(Annotations::LeaderVerticalAttachmentCenter);
    leader_data->SetStubSide(Annotations::LeaderSideInferred);
    note_factory->Leader()->Leaders()->Append(leader_data);

    // text size and justification
    note_factory->Style()->LetteringStyle()->SetGeneralTextSize(NOTE_SIZE);
    note_factory->Style()->LetteringStyle()->SetHorizontalTextJustification(Annotations::TextJustificationLeft);
    
    Annotations::Annotation::AssociativeOriginData note_origin;
    Annotations::Annotation* note_annotation(NULL);
    View* note_view(NULL);
    Point* note_point(NULL);

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
    Point3d note_location(anno_x, anno_y, 0.0);
    note_factory->Origin()->Origin()->SetValue(NULL, note_view, note_location);

    // create note
    NXObject* commit_result = note_factory->Commit();
    add_purgeable_object_to_export(commit_result);
    note_factory->Destroy();
    
    // switch back to modeling
    part->Drafting()->ExitDraftingApplication();

    return commit_result;
}

void DxfExportWorker::set_annotation_xy(double x_loc, double y_loc)
{
    anno_x = x_loc;
    anno_y = y_loc;
}

bool DxfExportWorker::add_purgeable_object_to_export(NXObject* object)
{
    purgeable_objects.push_back(object);

    return add_object_to_export(object);
}

void DxfExportWorker::purge_objects()
{
    for (NXObject* object : purgeable_objects)
        dxf_factory->ExportSelectionBlock()->SelectionComp()->Remove(object);
    
    purgeable_objects.clear();
}
