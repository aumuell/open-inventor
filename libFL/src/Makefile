#!smake
#
# Makefile for libFL
#
# $Header: /cvs/inventor/libFL/src/Attic/Makefile,v 1.1 2000/08/15 12:56:26 naaman Exp $
#
# This merely exists as a placeholder for now until if/when we can
# get clearance to release to source to this.  Right now a static
# version of the library is provided as a binary so that apps can
# compile and work.
#
DEPTH=.

all:: libFL.a


install: libFL.a
	@mkdir -p $(IVROOT)/usr/lib
	@cp libFL.a $(IVROOT)/usr/lib

clean clobber:
	rm -f $(IVROOT)/usr/lib/libFL.a
