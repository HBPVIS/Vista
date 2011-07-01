# $Id:
# supported platforms: IRIX, LINUX, SUNOS, HP-UX
# Windows has got an own makefile

all: transcoder
	
transcoder:
	cd build.transcoder ; $(MAKE) "MODE=DEBUG" "TOOLKIT=NONE" "TYPE=SO" ; cd ..
	cd build.transcoder ; $(MAKE) "MODE=RELEASE" "TOOLKIT=NONE" "TYPE=SO" ; cd ..

clean:
	cd build.transcoder; $(MAKE) "MODE=DEBUG" "TOOLKIT=NONE" "TYPE=SO" clean ; cd ..
	cd build.transcoder; $(MAKE) "MODE=RELEASE" "TOOLKIT=NONE" "TYPE=SO" clean ; cd ..

install:
	cd build.transcoder ; $(MAKE) -f Makefile.project "MODE=DEBUG"    "TYPE=SO"  install ; cd ..
	cd build.transcoder ; $(MAKE) -f Makefile.project "MODE=RELEASE"    "TYPE=SO"  install ; cd ..	
