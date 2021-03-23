#
# $Id: Makefile,v 3.0 1994/08/30 18:41:11 jjb Exp $
# $Log: Makefile,v $
# Revision 3.0  1994/08/30  18:41:11  jjb
# added expweight to list of files to compile.
#
# Revision 2.2  1994/07/25  18:47:02  jjb
# added copyright notice
#
#
CC = ${CCOMP} ${HF_I} ${HF_L} -I..
m = ${HFDIR}/misc

all: chfchan.o ctapwgts.o doppler.o iir32.o qphas.o weight.o four1.o qsc.o \
	q1.o besselweight.o expbess.o

chfchan.o: ../nb.h ../ifde.h ../airairchan.h $(m)/complex.h

ctapwgts.o: ../airairchan.h $(m)/complex.h

doppler.o: ../airairchan.h $(m)/complex.h

iir32.o: ../airairchan.h $(m)/complex.h

qphas.o: ../airairchan.h $(m)/complex.h

weight.o: ../airairchan.h $(m)/complex.h

besselweight.o: ../airairchan.h

expbess.o: 

*.c*.o:
	$(CC) -c *.c -o *.o
