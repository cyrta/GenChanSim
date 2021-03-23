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
CC = ${CCOMP} ${SIM_I} ${SIM_L} -I..
m = ${SIMDIR}/misc

all: chfchan.o ctapwgts.o doppler.o iir32.o qphas.o weight.o four1.o qsc.o q1.o expweight.o dataweight.o

chfchan.o: ../nb.h ../ifde.h ../JTCchan.h $(m)/complex.h

ctapwgts.o: ../JTCchan.h $(m)/complex.h

doppler.o: ../JTCchan.h $(m)/complex.h

iir32.o: ../JTCchan.h $(m)/complex.h

qphas.o: ../JTCchan.h $(m)/complex.h

weight.o: ../JTCchan.h $(m)/complex.h

expweight.o: ../JTCchan.h

dataweight.o: ../JTCchan.h

*.c*.o:
	$(CC) -c *.c -o *.o
