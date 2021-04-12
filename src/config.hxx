
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <vector>
#include <regex>

using namespace std;

namespace config
{
    enum layers {
        Profile =   1,
        Marking =   2,
        NoCut =     3,
        Detail =    4
    };

    extern string LOG_DIR;
    extern string DXF_CONFIG;
    extern string DXF_OUTPUT_DIR;

    extern double NOTE_SIZE;
    extern double NOTE_OFFSET;

    extern vector<string> JOB_PROP_KEYS;
    extern vector<string> MARK_PROP_KEYS;
    // regex WEB_PATTERN = regex( "(W[0-9]+)+" );
    // regex WEB_PARENT_PATTERN = regex( "(W[0-9]+){2,}" );

    extern map<string, string> PROPS;
    extern string SINGLE_BODY_EXPORT_NAME;
    extern map<string, int> WHITELISTED_SKETCHES;
    extern vector<string> BLACKLISTED_BODIES;

}

#endif
