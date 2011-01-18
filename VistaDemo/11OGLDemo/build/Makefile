# supported platforms: IRIX, LINUX, SUNOS, HP-UX
# Windows has got an own makefile

all: opensg

opensg:  debug_opensg  release_opensg

debug_opensg:
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=OPENSG"

release_opensg:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=OPENSG"

clean:
	$(MAKE) -f Makefile.project clean "MODE=RELEASE" "TOOLKIT=OSG"
	$(MAKE) -f Makefile.project clean "MODE=DEBUG" "TOOLKIT=OSG"
