# supported platforms: IRIX, LINUX, SUNOS, HP-UX
# Windows has got an own makefile

all: osg

osg:  debug_osg  release_osg

debug_osg:
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=OSG"
release_osg:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=OSG"

clean:
	$(MAKE) -f Makefile.project clean "MODE=RELEASE" "TOOLKIT=OSG"
	$(MAKE) -f Makefile.project clean "MODE=DEBUG" "TOOLKIT=OSG"

