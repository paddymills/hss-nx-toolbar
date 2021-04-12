
#include "HssAnnotation.hxx"
#include "config.hxx"

#include <NXOpen/NXObject.hxx>
#include <NXOpen/DraftingManager.hxx>
#include <NXOpen/SelectDisplayableObject.hxx>
#include <NXOpen/View.hxx>
#include <NXOpen/ViewCollection.hxx>
#include <NXOpen/Annotations_Annotation.hxx>
#include <NXOpen/Annotations_AnnotationManager.hxx>
#include <NXOpen/Annotations_LeaderBuilder.hxx>
#include <NXOpen/Annotations_LeaderData.hxx>
#include <NXOpen/Annotations_LeaderDataList.hxx>
#include <NXOpen/Annotations_Note.hxx>
#include <NXOpen/Annotations_OriginBuilder.hxx>

using namespace NXOpen;
using namespace std;

HssAnnotation::HssAnnotation()
{
    _annotation = nullptr;

    x = 0.0;
    y = 0.0;
}

void HssAnnotation::add_notes(map<string, string> n)
{
    map<string, string>::iterator itr;
    for (itr = n.begin(); itr != n.end(); ++itr)
        notes.push_back(itr->first + ": " + itr->second);
}

void HssAnnotation::add_notes(vector<NXString> n)
{
    notes.insert(notes.end(), n.begin(), n.end());
}

NXObject* HssAnnotation::create(Part* part)
{
    part->Drafting()->EnterDraftingApplication();
    part->Views()->WorkView()->UpdateCustomSymbols();
    part->Drafting()->SetTemplateInstantiationIsComplete(true);
    
    // turn off drawing layout (allows drafting tools in modeling)
    part->Drafting()->SetDrawingLayout(false);
    
    Annotations::DraftingNoteBuilder* note_factory;
    Annotations::SimpleDraftingAid* drafting_aid(NULL);
    note_factory = part->Annotations()->CreateDraftingNoteBuilder(drafting_aid);
    
    note_factory->Origin()->SetAnchor(Annotations::OriginBuilder::AlignmentPositionTopLeft);

    note_factory->Text()->TextBlock()->SetText(notes);
    note_factory->Origin()->Plane()->SetPlaneMethod(Annotations::PlaneBuilder::PlaneMethodTypeXyPlane);
    
    // set leader settings
    Annotations::LeaderData* leader_data = part->Annotations()->CreateLeaderData();
    leader_data->SetArrowhead(Annotations::LeaderData::ArrowheadTypeFilledArrow);
    leader_data->SetVerticalAttachment(Annotations::LeaderVerticalAttachmentCenter);
    leader_data->SetStubSide(Annotations::LeaderSideInferred);
    note_factory->Leader()->Leaders()->Append(leader_data);

    // text size and justification
    note_factory->Style()->LetteringStyle()->SetGeneralTextSize(config::NOTE_SIZE);
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
    Point3d note_location(x, y, 0.0);
    note_factory->Origin()->Origin()->SetValue(NULL, note_view, note_location);

    // create note
    _annotation = note_factory->Commit();
    note_factory->Destroy();
    
    // switch back to modeling
    part->Drafting()->ExitDraftingApplication();

    return _annotation;
}

NXObject* HssAnnotation::revise(Part* part)
{
    Annotations::DraftingNoteBuilder* note_factory;
    View* note_view(NULL);
    Point* note_point(NULL);

    Annotations::Note *note1(dynamic_cast<Annotations::Note *>(part->FindObject(_annotation->JournalIdentifier())));
    note_factory = part->Annotations()->CreateDraftingNoteBuilder(note1);
    note_factory->Text()->TextBlock()->SetCustomSymbolScale(1.0);
    note_factory->Origin()->SetInferRelativeToGeometry(true);
    note_factory->Origin()->SetInferRelativeToGeometry(true);
    
    Annotations::LeaderData *leader_data = part->Annotations()->CreateLeaderData();
    leader_data->SetArrowhead(Annotations::LeaderData::ArrowheadTypeFilledArrow);
    leader_data->SetVerticalAttachment(Annotations::LeaderVerticalAttachmentCenter);
    leader_data->SetStubSide(NXOpen::Annotations::LeaderSideInferred);

    note_factory->Leader()->Leaders()->Append(leader_data);
    note_factory->Origin()->SetInferRelativeToGeometry(true);

    // set note location
    Point3d note_location(x, y, 0.0);
    note_factory->Origin()->Origin()->SetValue(NULL, note_view, note_location);

    // change note
    note_factory->Text()->TextBlock()->SetText(notes);
    _annotation = note_factory->Commit();
    note_factory->Destroy();

    return _annotation;
}