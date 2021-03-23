#
# $Id: Makefile,v 2.2 1994/07/25 18:11:46 jjb Exp $
# $Log: Makefile,v $
# Revision 2.2  1994/07/25  18:11:46  jjb
# added Copyright notices
#
#
CC = ${CCOMP} ${SIM_L} ${SIM_I}
m = ${SIMDIR}/misc
l = ${SIMDIR}/cmdline

OFILES = $(m)/seed.o

atmoscal: atmoscal.o misc_obj lib_a
	$(CC) atmoscal.o $(OFILES) ${SIMLIBS} -o $@ -lm

atmoscal.o: $(l)/Cmdline.h

misc_obj:
	cd $(m); make

lib_a:
	cd $(l); make

*.c*.o:
	$(CC) -c *.c -o *.o
