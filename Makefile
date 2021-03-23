# makefile for all executables
CC = ${CCOMP} ${SIM_I} ${SIM_L} -I. -L.

all:
	(cd cmdline; make)
	(cd misc; make)
	(cd HF/AddDist/atmosphere/calibrate; make)
	(cd HF/AddDist/narrowband; make)
	(cd HF/AddDist/gaussnoise; make)
	(cd HF/PropChan; make)
	(cd HF/AddDist/atmosphere; make) 
	(cd MobileChan/AddDist/gaussnoise; make)
	(cd MobileChan/AddDist/ignition; make)
	(cd MobileChan/PathLoss; make)
	(cd MobileChan/PropChan/LandMobile/cellular/JTCmod; make)
	(cd MobileChan/PropChan/LandMobile/cellular/mixedmod; make)
	(cd MobileChan/PropChan/LandMobile/VHFUHF; make)
	(cd MobileChan/PropChan/AirMobile/airtoair; make)
	(cd MobileChan/PropChan/SatMobile; make)
	(cd Tools; make)
	(cd Tools/signal; make)
	(cd HF/src; make)

