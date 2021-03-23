#
# $Id: Makefile,v 0.0 1997/02/21 15:01:55 cmk Exp $
# $Log: Makefile,v $

CC = ${CCOMP} ${SIM_I} ${SIM_L} -I.
m = ${SIMDIR}/misc
l = ${SIMDIR}/cmdline

MISCOFILES = $(m)/cgauss48.o $(m)/rand.o $(m)/pathfile.o \
	$(m)/complex.o $(m)/fwritebuffer.o $(m)/seed.o

LOCALOFILES = 

OFILES = $(LOCALOFILES) $(MISCOFILES)

all: ignition

ignition: ignition.o $(LOCALOFILES) misc_obj lib_a
	$(CC) ignition.o $(OFILES) ${SIMLIBS} -o $@ -lm

ignition.o: ignition.h $(m)/complex.h $(m)/seed.h $(m)/pathfile.h \
	$(l)/Cmdline.h

misc_obj:
	cd $(m); make

lib_a:
	cd $(l); make

*.c*.o:
	$(CC) -c *.c -o *.o

