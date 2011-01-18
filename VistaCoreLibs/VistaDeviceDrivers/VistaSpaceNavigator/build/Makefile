# $Id:
# supported platforms: IRIX, LINUX, SUNOS, HP-UX
# Windows has got an own makefile

all: driver plugin

driver:
	cd build.driver ; $(MAKE) "MODE=DEBUG" "TOOLKIT=NONE" "TYPE=SO" ; cd ..

plugin: driver
	cd build.plugin ; $(MAKE) "MODE=DEBUG" "TOOLKIT=NONE" "TYPE=SO" ; cd ..

clean:
	cd build.driver ; $(MAKE) "MODE=DEBUG" "TOOLKIT=NONE" "TYPE=SO" clean ; cd ..
	cd build.plugin; $(MAKE) "MODE=DEBUG" "TOOLKIT=NONE" "TYPE=SO" clean ; cd ..

install:
	cd build.driver ; $(MAKE) -f Makefile.project "MODE=RELEASE"  "TYPE=SO"  install ; cd ..
	cd build.driver ; $(MAKE) -f Makefile.project "MODE=DEBUG"    "TYPE=SO"  install ; cd ..
       
	cd build.plugin ; $(MAKE) -f Makefile.project "MODE=RELEASE"  "TYPE=SO"  install ; cd ..
	cd build.plugin ; $(MAKE) -f Makefile.project "MODE=DEBUG"    "TYPE=SO"  install ; cd ..

clean: driver_clean plugin_clean
