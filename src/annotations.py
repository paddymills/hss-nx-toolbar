
import logging
import config

import NXOpen
import NXOpen.Annotations
import NXOpen.Drawings
import NXOpen.Features


logger = logging.getLogger(__name__)


def add_annotations(part, annotations, _x, _y):

    note_builder = _init_note_builder(part)
    
    # set note text
    note_builder.Text.TextBlock.SetText(annotations)
    
    # set note location
    x = _x + config.NOTE_OFFSET
    y = _y - config.NOTE_OFFSET
    location = NXOpen.Point3d(x, y, 0.0)
    note_builder.Origin.Origin.SetValue(NXOpen.TaggedObject.Null, NXOpen.View.Null, location)
    
    # create annotation
    note_builder.Commit()
    note_builder.Destroy()

    logger.debug("Annotations set at ({}, {})".format(x, y))


def _init_note_builder(part):

    # enter drafting
    NXOpen.Session.GetSession().ApplicationSwitchImmediate("UG_APP_DRAFTING")
    part.Drafting.EnterDraftingApplication()
    part.Views.WorkView.UpdateCustomSymbols()
    part.Drafting.SetTemplateInstantiationIsComplete(True)
    
    # turn off sheet display
    part.Drafting.SetDrawingLayout(False)

    # create note builder
    note_builder = part.Annotations.CreateDraftingNoteBuilder(NXOpen.Annotations.SimpleDraftingAid.Null)
    note_builder.Origin.SetInferRelativeToGeometry(True)
    note_builder.Origin.Anchor = NXOpen.Annotations.OriginBuilder.AlignmentPosition.TopLeft
    note_builder.Origin.Plane.PlaneMethod = NXOpen.Annotations.PlaneBuilder.PlaneMethodType.XyPlane
    
    # set note size
    note_builder.Style.LetteringStyle.GeneralTextSize = config.NOTE_SIZE

    return note_builder
