
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
    stringstream log_filename;

    try
    {
        SYSTEMTIME lt;
        GetLocalTime(&lt);

        log_filename << LOG_SAVE_DIR << "\\" << getenv("USERNAME");
        

        log_filename << setfill('0');

        // date
        log_filename << "_";
        log_filename << lt.wYear;
        log_filename << setw(2) << lt.wMonth;
        log_filename << setw(2) << lt.wDay;


        // time
        log_filename << "-";
        log_filename << setw(2) << lt.wHour;
        log_filename << setw(2) << lt.wMinute;
        log_filename << setw(2) << lt.wSecond;
        
        log_filename << ".log";

        ofile.open( log_filename.str() );
    }

    catch( const exception &ex )
    {
        ofile.open( string(LOG_SAVE_DIR).append("NONAME.log") );
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