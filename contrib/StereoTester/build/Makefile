# $Id:
# supported platforms: IRIX, LINUX, SUNOS, HP-UX
# Windows has got an own makefile

all: opensg

opensg:  debug_opensg  release_opensg

debug_opensg:
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=OPENSG"
	if [ ! -d ../../bin ]; then mkdir ../../bin; fi;
	cp `file --mime-type ../* | grep application/x-executable | sed -e 's/: *application\/x-executable//g'` ../../bin
release_opensg:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=OPENSG"
	if [ ! -d ../../bin ]; then mkdir ../../bin; fi;
	cp `file --mime-type ../* | grep application/x-executable | sed -e 's/: *application\/x-executable//g'` ../../bin

clean:
	$(MAKE) -f Makefile.project clean "MODE=DEBUG"   "TOOLKIT=OPENSG"
	$(MAKE) -f Makefile.project clean "MODE=RELEASE" "TOOLKIT=OPENSG"
