#
# $Id: Makefile,v 3.0 1994/08/30 19:11:12 jjb Exp $
# $Log: Makefile,v $
# Revision 3.0  1994/08/30  19:11:12  jjb
# added fracdelay.c fractapdelay.c and JTC and MACROCELL model
# versions of executables.
#
# Revision 2.2  1994/07/25  18:57:02  jjb
# added copyright notice
#
#
CC = ${CCOMP} ${SIM_I} ${SIM_L} -I.
t = ${SIMDIR}/MobileChan/PropChan/LandMobile/cellular/JTCmod/tapwgts
m = ${SIMDIR}/misc
l = ${SIMDIR}/cmdline

JTC_MODEL = 1
MACRO_MODEL = 2
DEBUG_MODEL = 3

MISCOFILES =$(m)/cgauss48.o $(m)/gauss48.o \
	$(m)/seed.o $(m)/pathfile.o

TAPOFILES = $(t)/chfchan.o $(t)/ctapwgts.o $(t)/doppler.o $(t)/iir32.o \
	$(t)/qphas.o $(t)/weight.o $(t)/four1.o $(t)/qsc.o $(t)/q1.o \
	$(t)/expweight.o $(t)/dataweight.o

JTCLOCALOFILES = JTCchan.o buffer.o cancoeff.o tapdelay.o fade.o \
	fracdelay.o fractapdelay.o 

MACROLOCALOFILES = macromobilechan.o buffer.o cancoeff.o tapdelay.o fade.o \
	fracdelay.o fractapdelay.o 

JTCOFILES =  $(JTCLOCALOFILES) $(TAPOFILES) $(MISCOFILES)

MACROOFILES =  $(MACROLOCALOFILES) $(TAPOFILES) $(MISCOFILES)

all: JTCchan 

JTCchan: $(JTCLOCALOFILES) tap_obj misc_obj lib_a
	$(CC) $(JTCOFILES) ${SIMLIBS} -o $@ -lm

buffer.o: JTCchan.h $(m)/complex.h

cancoeff.o: JTCchan.h

JTCchan.o: JTCchan.c JTCchan.h $(l)/Cmdline.h $(m)/complex.h $(m)/seed.h \
	$(m)/pathfile.h fracdly.h
	$(CC) -c -DMODEL=${JTC_MODEL} JTCchan.c -o JTCchan.o

tapdelay.o: JTCchan.h $(m)/complex.h

fractapdelay.o: fractapdelay.c JTCchan.h fracdly.h $(m)/complex.h 

fracdelay.o: fracdelay.c JTCchan.h fracdly.h $(m)/complex.h

fade.o: JTCchan.h

tap_obj:
	cd $(t); make

misc_obj:
	cd $(m); make

lib_a:
	cd $(l); make

*.c*.o:
	$(CC) -c *.c -o *.o
