#
# $Id: Makefile,v 2.2 1994/07/25 19:12:54 jjb Exp $
# $Log: Makefile,v $
# Revision 2.2  1994/07/25  19:12:54  jjb
# Added copyright notice.
#
#
CC = ${CCOMP} ${SIM_I} ${SIM_L}
m = ${SIMDIR}/misc
l = ${SIMDIR}/cmdline

MISCOFILES =  $(m)/cgauss48.o $(m)/pathfile.o

LOCALOFILES = gaussnoise.o

OFILES = $(LOCALOFILES) $(MISCOFILES)

gaussnoise: $(LOCALOFILES) misc_obj lib_a
	$(CC) $(OFILES) ${SIMLIBS} -o $@

gaussnoise.o: $(l)/Cmdline.h $(m)/complex.h $(m)/pathfile.h

misc_obj:
	cd $(m); make

lib_a:
	cd $(l); make

*.c*.o: 
	$(CC) -c *.c -o *.o
