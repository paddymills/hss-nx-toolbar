
import sys
import NXOpen

def main():
    log = NXOpen.Session.GetSession().LogFile

    log.WriteLine( "~~~~~~~~~~~~~~~~~~~~ HSS DXF EXPORT ({}) ~~~~~~~~~~~~~~~~~~~~".format(sys.argv[1]) )


if __name__ == "__main__":
    main()
