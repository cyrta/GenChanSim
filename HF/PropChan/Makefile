#
# $Id: Makefile,v 2.2 1994/07/25 19:29:19 jjb Exp $
# $Log: Makefile,v $
# Revision 2.2  1994/07/25  19:29:19  jjb
# added copyright notice.
#
#
CC = ${CCOMP} ${SIM_I} ${SIM_L} -I.
t = ${SIMDIR}/HF/PropChan/tapwgts
m = ${SIMDIR}/misc
l = ${SIMDIR}/cmdline

MISCOFILES = $(m)/cgauss48.o $(m)/gauss48.o $(m)/seed.o $(m)/pathfile.o

TAPOFILES = $(t)/hfchan.o $(t)/tapwgts.o $(t)/doppler.o $(t)/iir4.o \
	$(t)/qphas.o $(t)/weight.o $(t)/four1.o $(t)/qsc.o $(t)/q1.o 

LOCALOFILES = propchan.o buffer.o coeff.o tapdelay.o fade.o

OFILES = $(LOCALOFILES) $(TAPOFILES) $(MISCOFILES)

propchan: $(LOCALOFILES) tap_obj misc_obj lib_a
	$(CC) $(OFILES) ${SIMLIBS} -o $@ -lm

propchan.o: propchan.h $(l)/Cmdline.h $(m)/complex.h $(m)/seed.h \
	$(m)/pathfile.h

buffer.o: propchan.h $(m)/complex.h

coeff.o: propchan.h

tapdelay.o: propchan.h $(m)/complex.h

fade.o: propchan.h

tap_obj:
	cd $(t); make

misc_obj:
	cd $(m); make

lib_a:
	cd $(l); make

*.c*.o:
	$(CC) -c *.c -o *.o
