# supported platforms: IRIX, LINUX, SUNOS, HP-UX
# Windows has got an own makefile

all: debug release

debug:
	$(MAKE) -f Makefile.project "MODE=DEBUG"
release:
	$(MAKE) -f Makefile.project "MODE=RELEASE"

clean:
	$(MAKE) -f Makefile.project clean
