#
# $Id: Makefile,v 2.2 1994/07/25 18:30:41 jjb Exp $
# $Log: Makefile,v $
# Revision 2.2  1994/07/25  18:30:41  jjb
# added Copyright notice.
#
#
CC = ${CCOMP} ${SIM_I} ${SIM_L} -I.
l = ${SIMDIR}/cmdline
ofiles = fwritebuffer.o pathfile.o

all: cgauss48.o complex.o gauss.o lpgauss2.o \
qromb.o rand.o seed.o warning.o gauss48.o pathfile.o \
fwritebuffer.o

complex.o: complex.o complex.h

lpgauss2.o: complex.h

seed.o: seed.h

fwritebuffer.o: fwritebuffer.h

pathfile.o: pathfile.h

lib_a:
	cd $(l); make

*.c*.o:
	$(CC) -c *.c -o *.o
