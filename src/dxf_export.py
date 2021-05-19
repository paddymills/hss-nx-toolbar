
import config
import logging
import os

import NXOpen

class DxfExporter:

    def __init__(self, session, part_file):
        # replace self with DxfdwgCreator provided by session
        self._dxf = session.DexManager.CreateDxfdwgCreator()
        
        self._dxf.SettingsFile = config.DXF_CONFIG
        self._dxf.ExportData = NXOpen.DxfdwgCreator.ExportDataOption.Drawing
        self._dxf.ViewEditMode = True
        self._dxf.FlattenAssembly = True
        self._dxf.ExportAs = NXOpen.DxfdwgCreator.ExportAsOption.ThreeD
        self._dxf.ExportSelectionBlock.SelectionScope = NXOpen.ObjectSelector.Scope.SelectedObjects
        self._dxf.ExportFacesAs = NXOpen.DxfdwgCreator.ExportFacesAsOptions.PolylineMesh
        self._dxf.ProcessHoldFlag = True

        self._dxf.InputFile = part_file

        self.logger = logging.getLogger(__name__)

    def __del__(self):
        self._dxf.Destroy()

    def add(self, obj):
        # obj can be a list of objects
        self._dxf.ExportSelectionBlock.SelectionComp.Add(obj)
    
    def remove(self, obj):
        self._dxf.ExportSelectionBlock.SelectionComp.Remove(obj)

    def add_sketch(self, sketch):
        self.logger.info("Adding sketch: {}".format( sketch.Name ))

        self.add( sketch.GetAllGeometry() )

    def add_annotation(self, anno):
        self.logger.info("Adding Annotation: {}".format( anno.JournalIdentifier ))

        self.add( anno )

    def export_body(self, body, export_name, commit=True):
        
        self._dxf.OutputFile = os.path.join(config.DXF_OUTPUT_DIR, export_name)

        self.logger.info("Exporting body: {}".format( export_name ))

        try:
            body.Layer = config.Layers.PROFILE.value

            self.add( body )

            # delete part file if it exists
            # this seems to speed up export compared to overwriting files
            # also, it keeps from accumulating* _bk.dxf files
            if os.path.exists(self._dxf.OutputFile):
                self.logger.info("Removing existing .dxf file")
                os.remove(self._dxf.OutputFile)

            # generate dxf file
            if commit:
                self._dxf.Commit()

            self.remove( body )

        except Exception as err:
            raise Exception( "Failed to export body '{}' ({})".format(export_name, err) )
