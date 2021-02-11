
#include <iostream>
#include <vector>
#include <string>

//NX Open C++ header files
#include <NXOpen/Session.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/PartCollection.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/NXObjectManager.hxx>
#include <NXOpen/NXException.hxx>

//Open C Headers
#include <uf.h>
#include <uf_part.h>

using namespace NXOpen;

void processPart(Part *);

int main(int argc, char* argv[])
{
    try
    {
        /* Initialize the Open C API environment */
        int errorCode = UF_initialize();
        
        if ( 0 != errorCode )
            throw NXException::Create(errorCode);

        /* Initialize the NX Open C++ API environment */
        Session *theSession = Session::GetSession();

        char* partName = "Stacked_Haunch.prt";
        tag_t partTag;
        UF_PART_open(partName, &partTag, 0);

        std::vector <std::string> attrs;




        // PartCollection *partList = theSession->Parts();
        // Part *displayedPart = partList->Work();
        // processPart(displayedPart);
        
                        
        /* Terminate the Open C API environment */
        errorCode = UF_terminate();
    }
    catch (const NXOpen::NXException& ex)
    {
        std::cerr << "Caught exception " << ex.Message() << std::endl;
        return 1;
    }

    return 0;
}

void processPart(Part *partObject)
{
   BodyCollection *bodyList = partObject->Bodies();
   BodyCollection::iterator itr;
   std::cout << "Body list tag: " << bodyList->Tag() << std::endl;
//    for ( itr = bodyList->begin(); itr != bodyList->end(); ++itr )
//    {
//        std::cout << "Body found: " << itr.Name() << std::endl;
//    }
}
