#
# $Id: Makefile,v 2.2 1994/07/25 19:06:52 jjb Exp $
# $Log: Makefile,v $
# Revision 2.2  1994/07/25  19:06:52  jjb
# added copyright notice
#
#
CC = ${CCOMP} ${SIM_I} ${SIM_L} -I.
m = ${SIMDIR}/misc
l = ${SIMDIR}/cmdline

LOCALOFILES = narrowband.o narfunc.o iirfade.o iir.o

MISCOFILES = $(m)/cgauss48.o $(m)/rand.o $(m)/complex.o $(m)/fwritebuffer.o \
	$(m)/seed.o $(m)/pathfile.o

OFILES = $(LOCALOFILES) $(MISCOFILES)

narrowband: $(LOCALOFILES) narrowband.h misc_obj lib_a
	$(CC) $(OFILES) ${SIMLIBS} -lm -o $@

narrowband.o: narrowband.h iirfade.h $(l)/Cmdline.h $(m)/complex.h \
	$(m)/seed.h $(m)/pathfile.h

narfunc.o: narrowband.h $(m)/complex.h $(m)/fwritebuffer.h

iirfade.o: narrowband.h iirfade.h $(m)/complex.h

iir.o: filter.h $(m)/complex.h

misc_obj:
	cd $(m); make

lib_a:
	cd $(l); make

*.c*.o: 
	$(CC) -c *.c -o *.o
