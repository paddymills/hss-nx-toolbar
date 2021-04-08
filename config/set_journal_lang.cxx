
#include <uf_defs.h>
#include <NXOpen/NXException.hxx>
#include <NXOpen/Session.hxx>
#include <NXOpen/Preferences_SessionPreferences.hxx>
#include <NXOpen/Preferences_SessionUserInterface.hxx>

using namespace NXOpen;

extern "C" DllExport int ufusr_ask_unload()
{
    return (int)Session::LibraryUnloadOptionImmediately;
}

extern "C" DllExport void ufusr(char* param, int* retCode, int paramLen)
{
    Session* theSession = NXOpen::Session::GetSession();
    
    theSession->Preferences()->UserInterface()->SetJournalLanguage(Preferences::SessionUserInterface::JournalLanguageTypeCPlusPlus);
    theSession->Preferences()->UserInterface()->SetJournalFileFormat(Preferences::SessionUserInterface::JournalFileFormatTypeAscii);
}
