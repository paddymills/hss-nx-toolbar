
#include "LogStream.hxx"

#include <fstream>
#include <sstream>

#include <uf_defs.h>
#include <NXOpen/NXString.hxx>

using namespace NXOpen;
using namespace std;

ostream& operator<<(ostream& os_, NXString nxs)
{
    os_ << nxs.GetText();

    return os_;
}