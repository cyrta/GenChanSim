#
# $Id: Makefile,v 3.0 1994/08/30 19:11:12 jjb Exp $
# $Log: Makefile,v $
# Revision LL 0.0 Jan. 1996 lhr and cmk
# Implement the air to air channel model using the mixed model program 
# as a base.  This Makefile makes only the airairchan executable.
#
# Revision 3.0  1994/08/30  19:11:12  jjb
# added fracdelay.c fractapdelay.c and JTC and MACROCELL model
# versions of executables.
#
# Revision 2.2  1994/07/25  18:57:02  jjb
# added copyright notice
#
#
CC = ${CCOMP} ${SIM_I} ${SIM_L} -I.
t = ${SIMDIR}/MobileChan/PropChan/AirMobile/airtoair/tapwgts
m = ${SIMDIR}/misc
l = ${SIMDIR}/cmdline

AIRAIR_MODEL = 3

MISCOFILES =$(m)/cgauss48.o $(m)/gauss48.o \
	$(m)/seed.o $(m)/pathfile.o $(m)/complex.o

TAPOFILES = $(t)/chfchan.o $(t)/ctapwgts.o $(t)/doppler.o $(t)/iir32.o \
        $(t)/qphas.o $(t)/four1.o $(t)/qsc.o $(t)/q1.o \
		$(t)/expbess.o $(t)/besselweight.o 

AIRLOCALOFILES = airairchan.o buffer.o cancoeff.o tapdelay.o \
	fade.o fracdelay.o relpow.o fractapdelay.o

AIROFILES =  $(AIRLOCALOFILES) $(MISCOFILES) $(TAPOFILES)

all: airairchan

airairchan: $(AIRLOCALOFILES) libair_a misc_obj tap_obj
	$(CC) $(AIROFILES) ${AIRSIMLIBS} -o $@ -lm

buffer.o: airairchan.h $(m)/complex.h

cancoeff.o: airairchan.h

airairchan.o: airairchan.c airairchan.h $(l)/Cmdline.h $(m)/complex.h  \
	$(m)/pathfile.h fracdly.h 
	$(CC) -c -DMODEL=${AIRAIR_MODEL} airairchan.c -o airairchan.o

relpow.o: airairchan.h $(m)/complex.h

tapdelay.o: airairchan.h $(m)/complex.h

fractapdelay.o: fractapdelay.c airairchan.h fracdly.h $(m)/complex.h 

fracdelay.o: fracdelay.c airairchan.h fracdly.h $(m)/complex.h

fade.o: airairchan.h

tap_obj:
	cd $(t); make

misc_obj:
	cd $(m); make

libair_a:
	cd $(l); make libaircmd.a

*.c*.o:
	$(CC) -c *.c -o *.o

