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
# Otober 11th 1995 Linda Riehl.  This makefile 'make_macro' makes only
# the macromobilechan executable.
#
# Jan 1996 Edit makefile for new directory structure

CC = ${CCOMP} ${SIM_I} ${SIM_L} -I.
t = ${SIMDIR}/MobileChan/PropChan/LandMobile/VHFUHF/tapwgts
m = ${SIMDIR}/misc
l = ${SIMDIR}/cmdline

MACRO_MODEL = 2

MISCOFILES =$(m)/cgauss48.o $(m)/gauss48.o \
	$(m)/seed.o $(m)/pathfile.o

TAPOFILES = $(t)/chfchan.o $(t)/ctapwgts.o $(t)/doppler.o $(t)/iir32.o \
	$(t)/qphas.o $(t)/weight.o $(t)/four1.o $(t)/qsc.o $(t)/q1.o \
	$(t)/expweight.o $(t)/dataweight.o 


MACROLOCALOFILES = vhfuhfmixedchan.o buffer.o cancoeff.o tapdelay.o fade.o \
	fracdelay.o fractapdelay.o 

MACROOFILES =  $(MACROLOCALOFILES) $(TAPOFILES) $(MISCOFILES)

all: vhfuhfmixedchan

vhfuhfmixedchan: $(MACROLOCALOFILES) tap_obj misc_obj libvhf_a
	$(CC) $(MACROOFILES) ${VHFSIMLIBS} -o $@ -lm

buffer.o: mixedchan.h $(m)/complex.h

cancoeff.o: mixedchan.h

vhfuhfmixedchan.o: vhfuhfmixedchan.c mixedchan.h $(l)/Cmdline.h $(m)/complex.h $(m)/seed.h \
	$(m)/pathfile.h fracdly.h
	$(CC) -c -DMODEL=${MACRO_MODEL} vhfuhfmixedchan.c -o vhfuhfmixedchan.o

tapdelay.o: mixedchan.h $(m)/complex.h

fractapdelay.o: fractapdelay.c mixedchan.h fracdly.h $(m)/complex.h 

fracdelay.o: fracdelay.c mixedchan.h fracdly.h $(m)/complex.h

fade.o: mixedchan.h

tap_obj:
	cd $(t); make

misc_obj:
	cd $(m); make

libvhf_a:
	cd $(l); make libvhfcmd.a

*.c*.o:
	$(CC) -c *.c -o *.o

