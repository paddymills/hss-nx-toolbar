
import sys
import NXOpen

def main(test):
    log = NXOpen.Session.GetSession().LogFile

    log.WriteLine("~~~~~~~~~~~~~~~~~~~~ HSS DXF EXPORT ({}) ~~~~~~~~~~~~~~~~~~~~".format(test))


if __name__ == "__main__":
    main(sys.argv[1])
