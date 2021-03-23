#
# $Id: Makefile,v 2.2 1994/07/25 19:34:01 jjb Exp $
# $Log: Makefile,v $
# Revision 2.2  1994/07/25  19:34:01  jjb
# added copyright notice.
#
#
CC = ${CCOMP} ${SIM_I} ${SIM_L}
m = ${SIMDIR}/misc
l = ${SIMDIR}/cmdline

MISCOFILES = $(m)/pathfile.o $(m)/cgauss48.o

LOCALOFILES = signal.o

OFILES = $(LOCALOFILES) $(MISCOFILES)

signal: $(LOCALOFILES) misc_obj lib_a
	$(CC) $(OFILES) ${SIMLIBS} -o $@ -lm

signal.o: $(l)/Cmdline.h $(m)/complex.h $(m)/pathfile.h

misc_obj:
	cd $(m); make

lib_a:
	cd $(l); make

*.c*.o:
	$(CC) -c *.c -o *.o
