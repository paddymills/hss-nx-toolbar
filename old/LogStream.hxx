
#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include "LogBuffer.hxx"

#include <string>
#include <sstream>

#include <uf.h>
#include <uf_defs.h>
#include <NXOpen/NXString.hxx>

using namespace NXOpen;
using namespace std;

class LogStream : public ostream
{
    private:
        LogBuffer buffer;

    public:
        LogStream() : ostream(&buffer) {}
        void increase_indent() { buffer.inc(); }
        void decrease_indent() { buffer.dec(); }
        void reset_indent() { buffer.reset(); }
        void set_char(string s) { buffer.set_indent_seq(s); }

        void remove_old_logs() { buffer.remove_logs(); }
};

ostream& operator<<(ostream&, NXString);

#endif