#!gmake
#
# Makefile for SGI file support
#

CFLAGS += -DICC_SUPPORT
C++FLAGS += -DICC_SUPPORT

CFILES= close.c \
	filbuf.c \
	flsbuf.c \
	name.c \
	open.c \
	pix.c \
	rdwr.c \
	rle.c \
	row.c \
	$(NULL)

OFILES = close.o filbuf.o flsbuf.o name.o open.o pix.o rdwr.o rle.o row.o

all:: libimage.a

libimage.a: $(OFILES)
	$(RM) -f libimage.a
	$(AR) clq libimage.a $(OFILES)

libimage.so: $(OFILES)
	$(CC) -o $@ $(OFILES) -shared

install: all

clean clobber:
	$(RM) *.o *.d libimage.a libimage.so
