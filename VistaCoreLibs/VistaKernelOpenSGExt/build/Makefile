# $Id$
# supported platforms: IRIX, LINUX, SUNOS, HP-UX
# Windows has got an own makefile


so: debug_so release_so

static: debug release

all: so static

install: install_so

debug:
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=OPENSG" "TYPE=LIB"

release:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=OPENSG" "TYPE=LIB"

debug_so:
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=OPENSG" "TYPE=SO"

release_so:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=OPENSG" "TYPE=SO"

clean:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=OPENSG" "TYPE=LIB" clean
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=OPENSG" "TYPE=SO"  clean
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=OPENSG" "TYPE=LIB" clean
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=OPENSG" "TYPE=SO"  clean 


install_so:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=NONE" "TYPE=SO"  install
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=NONE" "TYPE=SO"  install 

install_static:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=NONE" "TYPE=LIB"  install
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=NONE" "TYPE=LIB"  install 
