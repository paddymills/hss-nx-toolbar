
import sys

import driver

import NXOpen

log = NXOpen.Session.GetSession().LogFile

log.WriteLine( "~~~~~~~~~~~~~~~~~~~~ HSS DXF EXPORT ({}) ~~~~~~~~~~~~~~~~~~~~".format(sys.argv[1]) )

driver.main()
