
#ifndef LOGBUFFER_H
#define LOGBUFFER_H

#include "config.hxx"
#include "filesystem.hxx"

#include <fstream>
#include <sstream>
#include <experimental/filesystem>

using namespace std;

class LogBuffer : public stringbuf
{
    private:
        int indent;
        bool needs_indent;
        string indent_seq;
        string filename;

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

        void remove_logs() { clear_dir_except( config::LOG_DIR, filename.c_str() ); }
};

#endif