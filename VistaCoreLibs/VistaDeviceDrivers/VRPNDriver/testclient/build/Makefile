# $Id:
# supported platforms: IRIX, LINUX, SUNOS, HP-UX
# Windows has got an own makefile

all: notk

notk: debug_notk release_notk

debug_notk:
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=NONE" "TYPE=BIN"
release_notk:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=NONE" "TYPE=BIN"

clean:
	$(MAKE) -f Makefile.project clean "MODE=DEBUG"   "TOOLKIT=NONE"
	$(MAKE) -f Makefile.project clean "MODE=RELEASE" "TOOLKIT=NONE"
