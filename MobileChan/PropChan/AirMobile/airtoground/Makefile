#
# $Id: Makefile,v 3.0 1994/08/30 19:11:12 jjb Exp $
# $Log: Makefile,v $
#
# Revision LL 0.0 Mar. 199 cmk
# Implement the air to ground channel model using the air to air program 
# as a base.  This Makefile makes only the airgroundchan executable.
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
t = ${SIMDIR}/MobileChan/PropChan/AirMobile/airtoground/tapwgts
m = ${SIMDIR}/misc
l = ${SIMDIR}/cmdline

AIRAIR_MODEL = 3

MISCOFILES =$(m)/cgauss48.o $(m)/gauss48.o \
	$(m)/seed.o $(m)/pathfile.o $(m)/complex.o

TAPOFILES = $(t)/chfchan.o $(t)/ctapwgts.o $(t)/doppler.o $(t)/iir32.o \
        $(t)/qphas.o $(t)/four1.o $(t)/qsc.o $(t)/q1.o \
		$(t)/expbess.o $(t)/besselweight.o 

AIRLOCALOFILES = airgroundchan.o buffer.o cancoeff.o tapdelay.o \
	fade.o fracdelay.o relpow.o fractapdelay.o

AIROFILES =  $(AIRLOCALOFILES) $(MISCOFILES) $(TAPOFILES)

all: airgroundchan

airgroundchan: $(AIRLOCALOFILES) libairgr_a misc_obj tap_obj
	$(CC) $(AIROFILES) ${AIRGRSIMLIBS} -o $@ -lm

buffer.o: airairchan.h $(m)/complex.h

cancoeff.o: airairchan.h

airgroundchan.o: airgroundchan.c airairchan.h $(l)/Cmdline.h $(m)/complex.h  \
	$(m)/pathfile.h fracdly.h 
	$(CC) -c -DMODEL=${AIRAIR_MODEL} airgroundchan.c -o airgroundchan.o

relpow.o: airairchan.h $(m)/complex.h

tapdelay.o: airairchan.h $(m)/complex.h

fractapdelay.o: fractapdelay.c airairchan.h fracdly.h $(m)/complex.h 

fracdelay.o: fracdelay.c airairchan.h fracdly.h $(m)/complex.h

fade.o: airairchan.h

tap_obj:
	cd $(t); make

misc_obj:
	cd $(m); make

libairgr_a:
	cd $(l); make libairgrcmd.a

*.c*.o:
	$(CC) -c *.c -o *.o

