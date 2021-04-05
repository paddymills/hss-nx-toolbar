
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#define _CRT_SECURE_NO_WARNINGS 1

#ifndef LOGBUFFER_H
#define LOGBUFFER_H

#include <fstream>
#include <sstream>
#include <experimental/filesystem>

using namespace std;

static const char *LOG_SAVE_DIR = "C:\\Users\\PMiller1\\git\\nx-dxf\\logs\\";

class LogBuffer : public stringbuf
{
    private:
        int indent;
        bool needs_indent;
        string indent_seq;

        ofstream ofile;

        void open_log_file();
    
    public:
        LogBuffer();
        ~LogBuffer() { ofile.close(); }

        void inc() { indent++; }
        void dec() { if ( indent > 0 ) indent--; }
        void reset() { indent = 0; }
        void set_indent_seq(string s) { indent_seq = s; }

        virtual int sync();
};

#endif