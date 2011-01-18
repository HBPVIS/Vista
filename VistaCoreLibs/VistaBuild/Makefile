so: VistaCoreLibs_so

clean: clean_cl

install: install_cl

VistaCoreLibs:
	${MAKE} -f Makefile.VistaCoreLibs debug release

VistaCoreLibs_so:
	${MAKE} -f Makefile.VistaCoreLibs debug_so release_so

clean_cl:
	${MAKE} -f Makefile.VistaCoreLibs clean

install_cl:
	${MAKE} -f Makefile.VistaCoreLibs install


OPENSG_ROOT ?= "/usr"
VISTA_OSTYPE ?= $(shell uname | tr '[:lower:]' '[:upper:]')
VISTA_HWARCH ?= $(shell uname -m | tr '[:lower:]' '[:upper:]')
env:
	@echo "OPENSG_ROOT:  ${OPENSG_ROOT}"
	@echo "VISTA_HWARCH: ${VISTA_HWARCH}"
	@echo "VISTA_OSTYPE: ${VISTA_OSTYPE}" 
