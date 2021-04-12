
#ifndef HSSPARTPROCESSING_H
#define HSSPARTPROCESSING_H

#include "LogStream.hxx"
#include "NxDxfExport.hxx"

#include <uf_defs.h>
#include <NXOpen/Session.hxx>
#include <NXOpen/Part.hxx>

using namespace NXOpen;
using namespace std;

class HssPartProcessing
{
    private:
        Session* session;
        NxDxfExport* exp;
        LogStream log;

    public:
        bool dry_run;

        HssPartProcessing() { 
            session = Session::GetSession();
            exp = nullptr;
        }

        void process_parts(vector<string>);
        void process_part(Part*);
        void process_part(BasePart* part) { process_part( dynamic_cast<Part*>(part) ); }
        void process_part(string s) { vector<string> _vec = { s }; process_parts(_vec); } 

        void handle_sketches(Part*);
        void handle_bodies(Part*);
        void handle_properties(Part*);
};

#endif
