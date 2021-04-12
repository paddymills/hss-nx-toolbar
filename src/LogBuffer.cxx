
#include "config.hxx"
#include "LogBuffer.hxx"

#include <iomanip>
#include <sstream>
#include <windows.h>

LogBuffer::LogBuffer()
{
    indent = 0;
    needs_indent = true;
    indent_seq = "\t";

    open_log_file();
}

void LogBuffer::open_log_file()
{
    stringstream log_name;

    try
    {
        SYSTEMTIME lt;
        GetLocalTime(&lt);

        log_name << config::LOG_DIR << "\\" << getenv("USERNAME");
        

        log_name << setfill('0');

        // date
        log_name << "_";
        log_name << lt.wYear;
        log_name << setw(2) << lt.wMonth;
        log_name << setw(2) << lt.wDay;


        // time
        log_name << "-";
        log_name << setw(2) << lt.wHour;
        log_name << setw(2) << lt.wMinute;
        log_name << setw(2) << lt.wSecond;
        
        log_name << ".log";

        filename = log_name.str();

        ofile.open( filename );
    }

    catch( const exception& ex )
    {
        ofile.open( string (config::LOG_DIR ).append("NONAME.log") );
    }
}

int LogBuffer::sync()
{
    // if after newline -> indent
    if ( needs_indent )
    {
        for ( int i = 0; i < indent; i++ )
            ofile << indent_seq;

        needs_indent = false;
    }

    // last character is newline -> next sync() needs indent
    if ( this->str().back() == '\n' )
        needs_indent = true;

    // write to output file
    ofile << this->str();

    // clear buffer
    this->str("");

    return 0;
}