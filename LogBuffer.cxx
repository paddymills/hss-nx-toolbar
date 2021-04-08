
#include "LogBuffer.hxx"

#include <iomanip>
#include <sstream>
#include <windows.h>

#include <experimental/filesystem>
namespace fs = experimental::filesystem;

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

        log_name << LOG_SAVE_DIR << "\\" << getenv("USERNAME");
        

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

void LogBuffer::remove_logs()
{
    for ( fs::directory_entry e : fs::directory_iterator( LOG_SAVE_DIR ) )
        if ( e.path() != filename )
            fs::remove( e.path() );
}