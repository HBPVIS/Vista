# $Id:
# supported platforms: IRIX, LINUX, SUNOS, HP-UX
# Windows has got an own makefile

all: notk

notk: debug_notk release_notk

debug_notk:
	$(MAKE) -f Makefile.project "MODE=DEBUG"   "TOOLKIT=NONE" "TYPE=SO"
	
release_notk:
	$(MAKE) -f Makefile.project "MODE=RELEASE" "TOOLKIT=NONE" "TYPE=SO"

clean:
	$(MAKE) -f Makefile.project clean "MODE=DEBUG"   "TOOLKIT=NONE"
	$(MAKE) -f Makefile.project clean "MODE=RELEASE" "TOOLKIT=NONE"

install:
	$(MAKE) -f Makefile.project install "MODE=DEBUG"   "TOOLKIT=NONE"
	$(MAKE) -f Makefile.project install "MODE=RELEASE" "TOOLKIT=NONE"