
from datetime import datetime
import logging

import config

import NXOpen
import NXOpen.Annotations
import NXOpen.Drawings
import NXOpen.Features


logger = logging.getLogger(__name__)


DT_FORMAT = r"%m-%d-%Y %I:%M:%S %p"


def add_annotations(part, annotations, _x, _y):

    note_builder = _init_note_builder(part)
    
    # set note text
    if type(annotations) is dict:
        annotations = ["{}: {}".format(k, v) for k, v in annotations.items() if v]

    
    annotations.append("EXPORTED: {}".format( datetime.now().strftime(DT_FORMAT) ))
    note_builder.Text.TextBlock.SetText(annotations)
    
    # set note location
    x = _x + config.NOTE_OFFSET
    y = _y - config.NOTE_OFFSET
    location = NXOpen.Point3d(x, y, 0.0)
    note_builder.Origin.Origin.SetValue(NXOpen.TaggedObject.Null, NXOpen.View.Null, location)
    
    # create annotation
    result = note_builder.Commit()
    note_builder.Destroy()

    logger.debug("Annotations set at ({}, {})".format(x, y))

    return result


def _init_note_builder(part):

    # enter drafting
    NXOpen.Session.GetSession().ApplicationSwitchImmediate("UG_APP_DRAFTING")
    part.Drafting.EnterDraftingApplication()
    part.Views.WorkView.UpdateCustomSymbols()
    part.Drafting.SetTemplateInstantiationIsComplete(True)
    
    # turn off sheet display
    part.Drafting.SetDrawingLayout(False)

    assocOrigin1 = NXOpen.Annotations.Annotation.AssociativeOriginData()

    # create note builder
    note_builder = part.Annotations.CreateDraftingNoteBuilder(NXOpen.Annotations.SimpleDraftingAid.Null)
    note_builder.Origin.SetInferRelativeToGeometry(True)
    note_builder.Origin.Anchor = NXOpen.Annotations.OriginBuilder.AlignmentPosition.TopLeft
    note_builder.Origin.Plane.PlaneMethod = NXOpen.Annotations.PlaneBuilder.PlaneMethodType.XyPlane
    note_builder.Style.LetteringStyle.HorizontalTextJustification = NXOpen.Annotations.TextJustification.Left
    
    # set note size
    note_builder.Style.LetteringStyle.GeneralTextSize = config.NOTE_SIZE

    return note_builder
