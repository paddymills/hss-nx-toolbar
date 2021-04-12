
#ifndef BASE_DIR
#define BASE_DIR "C:\\Users\\pmiller1\\git\\nx-dxf"
#endif

#include "config.hxx"

namespace config {
    string LOG_DIR( BASE_DIR "\\logs" );
    string DXF_CONFIG( BASE_DIR "\\config\\export.def" );
    string DXF_OUTPUT_DIR( "\\\\hssieng\\SNDataPrd\\DXF\\" );

    double NOTE_SIZE = 5.0;
    double NOTE_OFFSET = 10.0;

    vector<string> JOB_PROP_KEYS = { "JobNo", "JOB_NUMBER" };
    vector<string> MARK_PROP_KEYS = { "Mark", "PIECE_MARK" };

    // mapping of part property names to their annotation names
    map<string, string> PROPS = { { "DWG_NUMBER", "DRAWING" } };

    // if any body is named this, only that body will export
    string SINGLE_BODY_EXPORT_NAME = "_SN_PART";

    // strings to be found in sketch names
    map<string, int> WHITELISTED_SKETCHES {
            { "ZINC"     , 2 },
            { "DOR"      , 3 },
            { "NOCUT"    , 3 },
            { "NO_CUT"   , 3 }
        };

    // list of strings to be found in body names
    //  or names of features that construct body
    vector<string> BLACKLISTED_BODIES = { "SHIM" };
}
