CC = ${CCOMP} -I${SIMDIR}/misc -I.
m = ${SIMDIR}/misc
CFLAGS =

MISCOFILES =$(m)/cgauss48.o $(m)/gauss48.o \
	$(m)/seed.o

LOCALOFILES = main.o lognorm.o

OFILES =  $(LOCALOFILES) $(MISCOFILES)

all: main

main: $(LOCALOFILES) misc_obj
	$(CC) $(OFILES) -o $@ -lm

lognorm.o: $(m)/complex.h

main.o: main.c $(m)/complex.h $(m)/seed.h

misc_obj:
	cd $(m); make

*.c*.o:
	$(CC) -c *.c -o *.o




