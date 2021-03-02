
SHELL = /bin/sh


UGII_USERFCN       = "/mnt/c/Program Files/Siemens/NX1953/UGOPEN"
UGOpenLibs         = "$(UGII_USERFCN)/libufun.lib" "$(UGII_USERFCN)/libufun_cae.lib" "$(UGII_USERFCN)/libufun_cam.lib" "$(UGII_USERFCN)/libufun_die.lib" "$(UGII_USERFCN)/libufun_vdac.lib" "$(UGII_USERFCN)/libufun_weld.lib"
UGOpenInternalLibs = "$(UGII_USERFCN)/libugopenint.lib" "$(UGII_USERFCN)/libugopenint_cae.lib" "$(UGII_USERFCN)/libugopenint_cam.lib"

OBJS =  DxfExportWorker.o
INCLUDES = "$(UGII_USERFCN)"
LIBS = -l"$(UGII_USERFCN)"

${CXX} ${INCLUDES} DxfExportWorker.cxx $(UGOpenLibs) $(UGOpenInternalLibs)
