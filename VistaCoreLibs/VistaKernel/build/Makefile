# $Id$
# supported platforms: IRIX, LINUX, SUNOS, HP-UX
# Windows has got an own makefile

so: opensg_so

all: opensg_so opensg

install: install_so

opensg:  debug_opensg  release_opensg
opensg_so: debug_opensg_so release_opensg_so
notk: debug_notk release_notk

debug:   debug_opensg
release: release_opensg

debug_so: debug_opensg_so
release_so: release_opensg_so

debug_opensg:
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=OPENSG" "TYPE=LIB"
debug_opensg_so:
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=OPENSG" "TYPE=SO"
debug_notk:
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=NONE" "TYPE=LIB"
release_opensg:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=OPENSG" "TYPE=LIB"
release_opensg_so:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=OPENSG" "TYPE=SO"
release_notk:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=NONE" "TYPE=LIB"

clean:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=OPENSG" "TYPE=LIB" clean
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=OPENSG" "TYPE=LIB" clean
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=OPENSG" "TYPE=SO" clean
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=OPENSG" "TYPE=SO" clean

install_so:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=OPENSG" "TYPE=SO"  install
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=OPENSG" "TYPE=SO"  install 

install_static:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=OPENSG" "TYPE=LIB"  install
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=OPENSG" "TYPE=LIB"  install 
