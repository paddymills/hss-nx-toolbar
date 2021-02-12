
#include <iostream>
#include <vector>
#include <string>

//NX Open C++ header files
#include <ug_session.hxx>
#include <ug_part.hxx>
#include <ug_exception.hxx>

//Open C Headers
#include <uf.h>
#include <uf_part.h>

using namespace std;

int main(int argc, char* argv[])
{
    try
    {
        /* Initialize the Open C API environment */
        int errorCode = UF_initialize();
        
        if ( 0 != errorCode )
            throw UgException(errorCode);

        /* Initialize the NX Open C++ API environment */
        string fileName = "1190181A_G1A-web.prt";
        UgPart *part = UgPart::open(fileName);
    

        vector <string> attrs = part->askStringAttributeTitles();
        for ( const auto& value: attrs )
            cout << value << ": " << part->getStringAttributeValue(value) << endl;
                        
        /* Terminate the Open C API environment */
        errorCode = UF_terminate();
        
        if ( 0 != errorCode )
            throw UgException(errorCode);
    }
    catch (const UgException& ex)
    {
        cerr << "Caught exception code ";
        cerr << ex.askErrorCode();
        cerr << ": ";
        cerr << ex.askErrorText();
        cerr << endl;

        return 1;
    }

    return 0;
}