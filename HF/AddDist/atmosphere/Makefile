#
# $Id: Makefile,v 2.2 1994/07/25 18:01:55 jjb Exp $
# $Log: Makefile,v $
# Revision 2.2  1994/07/25  18:01:55  jjb
# Added copyright Notices.
#
#
CC = ${CCOMP} ${SIM_I} ${SIM_L} -I.
m = ${SIMDIR}/misc
l = ${SIMDIR}/cmdline

MISCOFILES = $(m)/cgauss48.o $(m)/rand.o $(m)/pathfile.o \
	$(m)/complex.o $(m)/fwritebuffer.o $(m)/qromb.o $(m)/seed.o

LOCALOFILES = atmfunc.o

OFILES = $(LOCALOFILES) $(MISCOFILES)

all: atmosphere cal_exec

atmosphere: atmosphere.o $(LOCALOFILES) misc_obj lib_a
	$(CC) atmosphere.o $(OFILES) ${SIMLIBS} -o $@ -lm

atmosphere.o: atmosphere.h $(m)/complex.h $(m)/seed.h $(m)/pathfile.h \
	$(l)/Cmdline.h

atmfunc.o: atmosphere.h $(m)/complex.h

misc_obj:
	cd $(m); make

lib_a:
	cd $(l); make

cal_exec:
	cd ${SIMDIR}/HF/AddDist/atmosphere/calibrate; make

*.c*.o:
	$(CC) -c *.c -o *.o

