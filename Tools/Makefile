#
# $Id: Makefile,v 2.2 1994/07/25 18:30:41 jjb Exp $
# $Log: Makefile,v $
# Revision 2.2  1994/07/25  18:30:41  jjb
# added Copyright notice.
#
#
CC = ${CCOMP} ${SIM_I} ${SIM_L} -I.
m = ${SIMDIR}/misc
l = ${SIMDIR}/cmdline
ofiles = $(m)/fwritebuffer.o $(m)/pathfile.o

all: fileconv filesum $(m)/cgauss48.o $(m)/complex.o $(m)/gauss.o $(m)/lpgauss2.o \
$(m)/qromb.o $(m)/rand.o $(m)/seed.o $(m)/warning.o $(m)/gauss48.o 

fileconv: fileconv.o $(ofiles) lib_a
	$(CC) fileconv.o $(ofiles) ${SIMLIBS} -o fileconv

filesum: filesum.o $(ofiles) lib_a
	$(CC) filesum.o $(ofiles) ${SIMLIBS} -o filesum -lm

complex.o: $(m)/complex.o $(m)/complex.h

fileconv.o: $(m)/complex.h $(l)/Cmdline.h $(m)/pathfile.h

filesum.o: $(m)/complex.h  $(l)/Cmdline.h $(m)/pathfile.h $(m)/fwritebuffer.h

lpgauss2.o: $(m)/complex.h

seed.o: $(m)/seed.h

fwritebuffer.o: $(m)/fwritebuffer.h

lib_a:
	cd $(l); make

*.c*.o:
	$(CC) -c *.c -o *.o
