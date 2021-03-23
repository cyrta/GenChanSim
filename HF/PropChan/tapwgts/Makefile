#
# $Id: Makefile,v 2.2 1994/07/25 19:20:47 jjb Exp $
# $Log: Makefile,v $
# Revision 2.2  1994/07/25  19:20:47  jjb
# added copyright notice
#
#
CC = ${CCOMP} ${SIM_I} ${SIM_L} -I..
m = ${SIMDIR}/misc

all: doppler.o four1.o hfchan.o  iir4.o q1.o qphas.o qsc.o tapwgts.o weight.o

doppler.o: ../propchan.h $(m)/complex.h

hfchan.o: ../propchan.h $(m)/complex.h

iir4.o: ../propchan.h $(m)/complex.h

qphas.o: ../propchan.h $(m)/complex.h

tapwgts.o: ../propchan.h $(m)/complex.h

weight.o: ../propchan.h $(m)/complex.h

*.c*.o:
	$(CC) -c *.c -o *.o
