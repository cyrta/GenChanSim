
# Generic Channel Simulator (GCS) 

This is mirror of: http://vhosts.eecs.umich.edu/genchansim/

code: [GCS.TAR as of February 1998](http://vhosts.eecs.umich.edu/genchansim/gcs.tar)
The version contained here is "unofficial". This has been lightly tested on three different UNIX systems during the winter of 1998. 
[details to build](./docs/gcssetup.txt)

**Background**    

The Generic Channel Simulator (GCS) is a software package designed to implement the wireless channel models developed by Dr. Phillip A. Bello under contracts MDA904-95-C-2078, DAAB07-93-C-N651, and DAAB07-94-C-H601. The modeling and simulation carried out in the initial contract DAAB07-93-C-N651, dealt only with the HF Ionosphere channel. The tapped delay line models and associated statistical channel models employed were of a generic nature and it was found that much of the software could be employed directly or modified slightly to model other radio channels, particularly the mobile cellular and non-cellular channels in the higher frequency bands. Thus under subsequent contracts the repertoire of simulated channels has expanded considerably. To reduce the burden to a GCS user in having to select from a large range of parameters, optional default scenarios identical to those discussed in the body of the final report of contract MDA904-95-C-2078 (see the documents page) have been introduced. Addtional default scenarios can be readily generated by the user as additional experimental data becomes available. For each channel separate programs are provided which simulate distortion (e.g. time variant multipath), and additive disturbances (e.g., interference) in addition to programs that allow estimation of path loss.The software has been programmed in ANSI C and developed in a Unix environment on a SUN workstation. Both Motif Windows and command -line scripts can be used for entry of parameters.

At the present time the GCS simulates the following channels: Cellular/PCS (Macrocell and Microcell), VHF/UHF Mobile-Mobile and Mobile-Base Station, VHF/UHF Air-Air and Air-Ground, Land Mobile Satellite, Indoor Channels, and HF Ionospheric. The GCS should provide a useful tool in the comparative evaluation of communication and detection techniques over both military and non-military wireless channels.


**Acknowledgments**    

A number of individuals have contributed to the software development. The initial work, confined to the WBHF channel, was carried out by Richard Coutts ( with some contributions by Mark Sandler) when they worked at Mitre Corp. This was followed by the JTC and Mixed Channel models of the cellular channels programmed by Christopher Nissen and Jeffrey Blanchard of the Mitre Corp. The most recent software effort has been carried out by Dr. Catherine Keller (with some support by Linda Riehl) of MIT Lincoln Laboratory. Dr. Keller has written a user's manual and a link to this manual is provided in the documents section of the Home Page. 


**References**    

-  The [GCS Final Report](./docs/FinalReport.pdf) by P.A. Bello describes the Generic Channel Simulator in great detail.

- P.A. Bello's [MILCOM'97 Paper](./docs/milcom97paper.pdf) contains an overview of the Generic Channel Simulator. 

-----
                Generic Channel Simulator README PAGE
                                 by
                        P. A. Bello, BELLO, INC.
                                and
                 C. M. Keller, MIT Lincoln Laboratory
                   

INTRODUCTION

    The Generic Channel Simulator (GCS) is a software package developed
for the DOD. The GCS is an outgrowth of Wideband HF (WBHF) Ionospheric
modeling and simulation work carried out under Contract
DAAB07-93-C-N651. [1] documents the basis for the HF channel models
developed. The tapped delay line and associated statistical models
employed were of a generic nature and it was found that much of the
software could be employed directly or modified to model other radio
channels, particularly the mobile cellular and non-cellular channels
in the higher frequency bands.

    Thus, under Contract DAAB07-94-C-H601 modifications and additions
were made to allow the simulation of macrocell and microcell channels [2]. A
subset of the software was used to simulate the models adopted by the JTC
as a commercial standard for testing of PCS/Cellular modems. However, the
GCS includes a more realistic simulation of PCS/Cellular channels than the
JTC simulation using the Mixed Discrete/Scatter Model. In addition,
the most recent model development for the GCS [3], has made the
channel simulations more versatile and has increased the types of
channels simulated. Current with the work carried out in [3], software
development and  modifications of the GCS have been carried out by MIT
Lincoln Laboratory under a separate contract [4]. Prior software
development was carried out by the Mitre Corp[2][5].

    At the present time the GCS simulates the following channels:
Cellular/PCS (Macrocell and Microcell), VHF/UHF Mobile-Mobile and
Mobile-Base Station, VHF/UHF Air-Air and Air-Ground, Land Mobile Satellite,
Indoor Channels, and HF Ionospheric. The GCS should provide a useful tool
in the comparative evaluation of communication and detection techniques
over both military and non-military wireless channels. 

DIRECTORY STRUCTURE AND SOFTWARE NOMENCLATURE

    The information that follows is included to help the user become
familiar with the directory structure of the software package and to provide
instructions on how to enter parameters and run programs.  This is a
mere subset of the information that is contained in [4].

    Below is a directory tree diagram of the current software package,
followed by a brief explanation of the role of each directory. 
```
  GenChanSim
  `----HF (hfc)
  |    `----AddDist
  |    |    `----atmosphere (atmosphere)
  |    |    |    `----calibrate (atmoscal)
  |    |    `----gaussnoise (gaussnoise)
  |    |    `----narrowband (narrowband)
  |    `----PathLoss *
  |    `----PropChan (propchan)
  |         `----scenarios (whichhf)
  |    `----src
  `----MobileChan
  |    `----AddDist
  |    |    `----cochannel *
  |    |    `----gaussnoise (gaussnoise)
  |    |    `----ignition (ignition)
  |    `----PathLoss (cost231hata, cost231walike, hata, nlosbelrfblk, ...)
  |    `----PropChan
  |         `----SatMobile (lin, lutz, flatfade)
  |         |    `----scenarios
  |         `----AirMobile
  |         |    `----airtoair (airairchan)
  |         |         `----scenarios (whichscen)
  |         |    `----airtoground (airgroundchan)
  |         |         `----scenarios (whichscen)
  |         `----LandMobile
  |              `----cellular
  |              |    `----JTCmod (JTCchan)
  |              |    |    `----scenarios
  |              |    `----mixedmod (mixedchan)
  |              |         `----scenarios
  |              |              `----indoor (whichscen)
  |              |              `----macro (whichscen)
  |              |              `----micro (whichscen)
  |              `----VHFUHF (vhfuhfmixedchan)
  |                   `----scenarios
  |                        `----mobiletobase (whichscen)
  `----Sat                 `----mobiletomobile (whichscen)
  |    `----AddDist *
  |    `----PathLoss *
  |    `----PropChan *
  `----Tools (fileconv, filesum, linkbud)
  |    `----signal (signal)
  `----cmdline
  `----matlab
  `----misc
  `----xapplres
--------------------------------------------------------------------
( . ) == executable program within the directory.
  *   == empty directory.

--------------------------------------------------------------------


directory   description
---------   ----------
GenChanSim  Base directory containing all the required software (excluding 
            standard UNIX libraries) to compile and run the GCS code.
 
  HF          Base directory for all HF channel simulator software.
              Also contains a top-level GUI program 'hfc' with a menu
              of HF programs.

    AddDist     Additive disturbance simulators.

      atmosphere  Contains the atmospheric interference simulator.  Subdirectory 
                  'calibrate' contains software for calibrating values of Vd and 
                  sigma.
      gaussnoise  Contains the Gaussian noise generator.
      narrowband  Contains the narrowband interference simulator.

    PathLoss    To contain pathloss models for HF

    PropChan    Contains the HF propagation channel simulator
                'propchan' and the subdirectory 'scenarios' with
                script files to run specific models, the the program
                'whichhf' to help the user select one  of the
                scenarios.  A subdirectory 'tapwgts' contains software
                used in generating the tap weight coefficients.

    src	        Contains the source code for the MOTIF front end that selects
	        channel modeling software. NOTE: not used for pathloss or receive
	        SNR programs.

  MobileChan  Base directory for all mobile channel simulator software.

    AddDist     Additive disturbance simulators.

      cochannel   Will Contain the cochannel interference simulator.
      gaussnoise  Contains the Gaussian noise generator 'gaussnoise.'
      ignition    Contains the ignition noise simulator 'ignition.'

    PathLoss    Contains pathloss models for the mobile channel,
                'cost231hata,' 'cost231walike,' 'hata,'
                'nlosbelrfblk,' 'ploss\_jtc,' 'qkarea,' 'snr2loss,'
                and 'unobdirurbhf.' To also contain
                satellite-to-ground path-loss models.

    PropChan    The base directory for the mobile propagation channel
                simulators.

      SatMobile   Contains the satellite-to-ground propagation channel
                  simulators 'lin,' 'lutz,' 'flatfade,' and the
                  subdirectory 'scenarios' with script files to run
                  specific models.
      AirMobile   The base directory for the VHF/UHF air mobile
                  propagation channel simulators. 
        airtoair    Contains the air-to-air channel simulator.
                    'airairchan' and the subdirectory 'scenarios' with
                    script files to run specific models, and the
                    program 'whichscen' to help the user select one of
                    the scenarios.
        airtoground Contains the air-to-ground channel simulator
                    'airgroundchan' and the subdirectory 'scenarios' with
                    script files to run specific models, and the
                    program 'whichscen' to help the user select one of
                    the scenarios.
      LandMobile  The base directory for the mobile communication
                  channel simulators. 
        cellular    Contains the cellular and pcs channel simulators.
          JTCmod      Contains the JTC channel simulator 'JTCchan' and a
                      subdirectory 'scenarios' with script files to
                      run specific models. 
          mixedmod    Contains the mixed discrete/scatter path
                      channel simulator 'mixedchan' and subdirectories
                      'scenarios/indoor,' 'scenarios/macro,' and
                      'scenarios/micro' with script files to run
                      specific models, and the program ``whichscen''
                      to help the user select one of the scenarios. 

        VHFUHF      Contains the VHF/UHF mobile-to-mobile program
                    'vhfuhfmixedchan' and the subdirectories
                    'scenarios/mobiletobase' and
                    'scenarios/mobiletomobile' with script files to
                    run specific models, and the program 'whichscen' 
                    to help the user select one of the scenarios.

  Sat         The base directory for the conventional
              (non-land-mobile) satellite channel simulators.

  Tools       Contains tools used with any type of simulator including
              a link budget program 'linkbud' that calculates receiver
              SNR.
    fileconv    Contains the tool 'fileconv' to convert a simulator
                output file to MATLAB format.
    filesum     Contains the tool 'filesum' to add simulator output files.
    signal      Contains a complex signal generator 'signal' for steps and
                impulses that can be used for simulator input signals.

  cmdline     Contains software routines used in the development of
              the Motif interfaces.

  matlab      Contains scripts for data reduction.

  misc        Contains miscelaneous software and common support software for
              many of the simulator routines.  

  xapplres    Contains resource files for the motif interface.
              'cmdline' is the default resource file.

```

    A brief list and description is given below on executable software
provided.  Many executables are given the same name as the directory
in which they reside.  For information on software parameters, please
refer to [4].  A 'usage' line for a program can be obtained at the
UNIX prompt simply typing the name of the module, e.g.,
```
% fileconv

returns 

Usage: fileconv -i Input_File [-o Output_File] [-m Matlab_Format] [-a ASCII_Form
at] [-win]
	
software       description
--------       ------------------
signal	       Complex signal generator (impulse, ones, sine)

fileconv       Convert binary complex file to either ascii or a Matlab 3.5 
               '.mat' format.

filesum        Weight and sum binary complex input files to one binary complex
               output file.  Up to five input files and gain may be entered.

loadcomplex.m  Matlab script for reading complex data file into matlab 4.0.

loadfloat.m    Matlab script for reading floating-pt data file into matlab 4.0.

propchan       HF propogation channel model.

atmosphere     HF atmospheric noise simulator.

narrowband     HF narrowband interference generator.

gaussnoise     Gaussian noise generator.

hfnoise        background Gaussian noise simulator.

hfc	       MOTIF front end to select HF channel simulation software.
	       This front end is not needed if you know which executable
	       to use.

ignition       VHF/UHF ignition additive disturbances simulator.

mixedchan      Mobile channel mixed discrete/scatter path model.

vhfuhfmixedchan VHF/UHF mixed discrete/scatter path model simulator.

JTCchan        Mobile channel JTC model.

airairchan     VHF/UHF air-to-air channel simulator.

airgroundchan  VHF/UHF air-to-ground channel simulator.

flatfade       flat fading channel simulator.

lutz           Lutz land-mobile satellite channel simulator.

lin            Lin land-mobile satellite channel simulator.

cost231hata    COST 231 hata model for 1500-2000MHz urban.

cost231walike  COST 231 Walfish and Ikegami path loss model.

hata	       Hata path loss model.

linkbud	       Receive SNR link budget program.

snr2loss       Receive SNR link budget program, but takes SNR as input
	       and outputs the corresponding path loss.

ploss_jtc      JTC path loss model.

qkarea	       Longley-Rice "Quick Area" path loss model program from
	       "A Guide to the Use of the ITS Irregular Terrain Model
	       in the Area Prediction Mode".

unobdirurbhr   1st order dielectric canyon 
	       (Unobstructed direct path urban high rise)
	        path loss model.

nlosbelrfblk	non-LOS below the roof ( 1st order road guided and 
		COST231-Walfisch-Ikegami ) with Tx (base) at center
	        of a block.

nlosbelrfint	non-LOS below the roof ( 1st order road guided and 
		COST231-Walfisch-Ikegami ) with Tx (base) at center
	        of an intersection.
```

RUNNING SOFTWARE

   To run the software the user can either use aliases or add an entry into
their path list.

    For convenience, numerous aliases have been defined to allow the user
to move about directories and to run programs more easily.  The
files defining the aliases are in the same directory as this ReadMe
file, called MOTIF.alias and NO_MOTIF.alias.  It is recommended that
the user defines a single alias in his/her .cshrc file that sources
the appropriate one of these .alias files when called.  For example,
the user might include the line 

alias simstart  'source /pathtogenchansim/GenChanSim/MOTIF.alias'

in his/her .cshrc file, where pathtogenchansim is the path to the
loaded software.  Then, the user can go to any desired working
directory, type simstart, and then have all of the alias defined in
MOTIF.alias at his/her fingertips.

    The user will have to edit MOTIF.alias and NO_MOTIF.alias to contain
the correct pathtogenchansim and path to Motif and libraries.  Then,
the user should get into the GenChanSim directory, and type "make" so
that all of the software will be compiled.

Good-luck.


REFERENCES

[1] P.A.Bello, Wideband HF Propagation, Narrowband Interference, and
Atmospheric Noise Models for Link Performance Evaluation, MITRE Technical
Report MT 93B0000086, July 1993.

[2] P.A.Bello, C.A.Nissen, and J.J.Blanchard, Generic Channel Simulator,
Draft Final Report on Project 8632L, Mitre Corp., September 30, 1994.

[3] P.A.Bello, Generic Channel Simulator, BELLO, Inc. Report BTR 103, Final
Report  under Contract MDA904-95-C-2078. February, 10,1997.

[4] C.M.Keller, Generic Channel Simulator Software, MIT Lincoln
Laboratory Project Report AST-46, August 1997.

[5] R. Coutts, Software Wideband HF Channel Simulation, MITRE Report
MTR93B0000122, The MITRE Corp, Bedford, Massachusetts, October 1993.




