
#include <string>
#include <map>
#include <vector>
#include <regex>

using namespace std;

namespace config {
    const enum layers {
        Profile     = 1,
        Marking     = 2,
        NoCut       = 3,
        Detail      = 4
    };

    const char* BASE_DIR = "C:\\Users\\PMiller1\\git\\nx-dxf";

    const char* LOG_DIR = string(BASE_DIR).append("\\logs").c_str();
    const char* DXF_CONFIG = string(BASE_DIR).append("\\config\\export.def").c_str();
    const char* DXF_OUTPUT_DIR = "\\\\hssieng\\SNDataPrd\\DXF\\";

    const double NOTE_SIZE = 5.0;
    const double NOTE_OFFSET = 10.0;

    const vector<char*> JOB_PROP_KEYS = { "JobNo", "JOB_NUMBER" };
    const vector<char*> MARK_PROP_KEYS = { "Mark", "PIECE_MARK" };
    const regex WEB_PATTERN = regex( "(W[0-9]+)+" );
    const regex WEB_PARENT_PATTERN = regex( "(W[0-9]+){2,}" );

    // mapping of part property names to their annotation names
    const map<string, string> properties {
        { "DWG_NUMBER", "DRAWING" }
    };

    // strings to be found in sketch names
    const map<string, int> WHITELISTED_SKETCHES {
            { "ZINC"     , 2 },
            { "DOR"      , 3 },
            { "NOCUT"    , 3 },
            { "NO_CUT"   , 3 }
        };

    // list of strings to be found in body names
    //  or names of features that construct body
    const vector<string> BLACKLISTED_BODIES = { "SHIM" };

}