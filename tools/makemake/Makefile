#!smake
#   toolkits/Inventor/tools/makemake/Makefile
#
#   $Revision: 1.1 $
#

DEPTH = ../../lib/

include $(DEPTH)../make/so_commondefs

TARGETDIR	= ./obj
TARGETS		= $(TARGETDIR)/makemake
COMMONPREF	= makemake
LDIRT		= $(TARGETDIR)/*.o

default: do_all

include $(COMMONRULES)

do_all: maketd
	@echo "\n\n==== Make default for src"
	@( cd src; $(MAKE) default )

$(COMMONTARGS): $(COMMONPREF)$$@

maketd: $(_FORCE)
	@if [ ! -d $(TARGETDIR) ]; then mkdir $(TARGETDIR) ; fi

install: default
	@echo "need to make install"
