#
# $Id: Makefile,v 2.2 1994/07/25 19:36:41 jjb Exp $
# $Log: Makefile,v $
# Revision 2.2  1994/07/25  19:36:41  jjb
# added copyright notice.
#
#
CC = ${CCOMP} ${HF_I} ${HF_L} -I.
m = ${HFDIR}/misc
l = ${HFDIR}/cmdline

LOCALOFILES = hfc.o

MISCOFILES = $(m)/pathfile.o

OFILES = $(LOCALOFILES) $(MISCOFILES)

hfc: $(LOCALOFILES) misc_obj lib_a
	$(CC) $(OFILES) ${HFLIBS} -o $@ -lm

hfc.o: $(l)/Cmdline.h $(m)/pathfile.h

misc_obj:
	cd $(m); make

lib_a:
	cd $(l); make

*.c*.o:
	$(CC) -c *.c -o *.o
