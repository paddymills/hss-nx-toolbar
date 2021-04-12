
#include "LogStream.hxx"

#include <fstream>
#include <sstream>

#include <uf_defs.h>
#include <NXOpen/NXString.hxx>
#include <NXOpen/Sketch.hxx>
#include <NXOpen/Body.hxx>

using namespace NXOpen;
using namespace std;

ostream& operator<<(ostream& os_, NXString nxs)
{
    os_ << nxs.GetText();

    return os_;
}

ostream& operator<<(ostream& os_, Sketch* sketch)
{
    os_ << sketch->Name();

    return os_;
}

ostream& operator<<(ostream& os_, Body* body)
{
    os_ << body->Name() << "(" << body->JournalIdentifier() << ")";

    return os_;
}
