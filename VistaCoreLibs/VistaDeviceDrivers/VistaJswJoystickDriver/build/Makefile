# $Id:
# supported platforms: IRIX, LINUX, SUNOS, HP-UX
# Windows has got an own makefile

all: driver plugin transcoder

driver:
	cd build.driver ; $(MAKE) "MODE=DEBUG" "TOOLKIT=NONE" "TYPE=SO" ; cd ..

plugin: driver
	cd build.plugin ; $(MAKE) "MODE=DEBUG" "TOOLKIT=NONE" "TYPE=SO" ; cd ..
	
transcoder: driver
	cd build.transcoder ; $(MAKE) "MODE=DEBUG" "TOOLKIT=NONE" "TYPE=SO" ; cd ..



clean_driver:
        cd build.driver ; $(MAKE) -f Makefile.project clean "MODE=DEBUG"   "TOOLKIT=NONE" "TYPE=SO"

clean_plugin:
	cd build.plugin ; $(MAKE) -f Makefile.project clean "MODE=RELEASE" "TOOLKIT=NONE" "TYPE=SO"

clean_transcoder:
	cd build.transcoder ; $(MAKE) -f Makefile.project clean "MODE=RELEASE" "TOOLKIT=NONE" "TYPE=SO"

clean: clean_driver clean_plugin clean_transcoder
