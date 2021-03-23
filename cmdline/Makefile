#
# $Id: Makefile,v 2.2 1994/07/25 18:18:32 jjb Exp $
# $Log: Makefile,v $
# Revision 2.2  1994/07/25  18:18:32  jjb
# Added copyright notice
#
#
CC = ${CCOMP} ${SIM_I} ${SIM_L} -I. -L.

libcmd.a: cmdline.c Cmdline.h
	$(CC) -c cmdline.c
	ar rcv $@ cmdline.o
	ranlib libcmd.a
	rm cmdline.o

libmixedcmd.a: mixedcmdline.c Cmdline.h
	$(CC) -c mixedcmdline.c
	ar rcv $@ mixedcmdline.o
	ranlib libmixedcmd.a
	rm mixedcmdline.o

libvhfcmd.a: vhfcmdline.c Cmdline.h
	$(CC) -c vhfcmdline.c
	ar rcv $@ vhfcmdline.o
	ranlib libvhfcmd.a
	rm vhfcmdline.o

libaircmd.a: aircmdline.c Cmdline.h
	$(CC) -c aircmdline.c
	ar rcv $@ aircmdline.o
	ranlib libaircmd.a
	rm aircmdline.o

libairgrcmd.a: airgrcmdline.c Cmdline.h
	$(CC) -c airgrcmdline.c
	ar rcv $@ airgrcmdline.o
	ranlib libairgrcmd.a
	rm airgrcmdline.o

libflatcmd.a: flatcmdline.c Cmdline.h
	$(CC) -c flatcmdline.c
	ar rcv $@ flatcmdline.o
	ranlib libflatcmd.a
	rm flatcmdline.o

democmd: democmd.c libcmd.a
	$(CC) democmd.c -o $@ -lcmd -lXm -lX11 -lXt

calc: calc.c libcmd.a
	$(CC) calc.c -o $@ -lcmd -lXm -lX11 -lXt -lm
