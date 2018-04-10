/*
**************************************************************************************************
		Particles development version - preparing new features for MotionComposer 3.0

		Copyright (C) 2017  Andreas Bergsland
		Correspondence: andreas.bergsland@ntnu.no
**************************************************************************************************
RUN:
In cmd: csound -odac --omacro:Path=E:\\Particles_MC3.0\\ -b 256  -B 512	 E:/Particles_MC3.0/Particles_Main.csd

TODO: 
Interaction:
- Prize sounds: When you break dishes, there is a voice saying "ah aaah" afterwards, and a subsequent
attenuation of all sound
- enable different mappings of activation of prize sounds: jump, kick, punch up, arms over head 
(changing slowly until the tipping point, accumulated activity
- Could also consider including drone again, but maybe in a way that reflects the particles more


Functionality
- Make it possible to have an arbitrary number of partikkel instruments to enable 2 player version + chair
(i.e. up to 4 instances)
- Enable an arbitrary number of effect parameters, e.g. by writing all MC parameters into an array
- Improve import of rhythms from MIDI-files (made in Ableton Live)
- Change so that import is time based instead of step based (could make a better groove (groove templates?)


Internal/houskeeping
- Overall: Implement more modularity (UDOs, instruments) so that it can run more flexibly with
the main script mainly including instrument definitions living in a separate folder of different
versions.
- Try to get rid of python pre-processing
- Also it could be good to maintain backward compatibility with both MC2.0 patches and MC data so that
one can easily compare and even perhaps combine.
- Make option so that particle instrument can run with single sound file ftables instead

Minor adjustments
- Longs should not repeat, should get different sound each time
**************************************************************************************************
*/
<CsoundSynthesizer>
<CsOptions>
;-odac --omacro:Path=E:\\Particles_MC3.0\\ -B512	-b256
</CsOptions>
<CsInstruments>
;**************************************************************************************************
sr		=		48000	; Sample rate matches sound files
ksmps		=		64	; Samples per control period - gives a kr period of 1.33ms
nchnls		=		2	; Stereo
0dbfs		=		1	; Maximum 0dB reference level
;**************************************************************************************************
;		INITIATE GLOBAL VARIABLES AND CHANNELS, FTABLES AND GUI
;**************************************************************************************************

#define	ALWAYSON		#9999999#


; Initiate python
		pyinit

/*_____________________________________________________________________________________________
 INITIATE OSC PORTS FOR COMMUNICATION WITH CONTROL (CM) AND TRACKING (TM) MODULES
ME sends to TM on 6061
ME sends to CM on 6065
ME listens TM on 6160
ME listens CM on 6560
*/

giCMserver	OSCinit		6560			; New ports for MC3.0  	
giTMserver	OSCinit		6160
giME2CM	=			6065
giME2TM	=			6061

;gS_IP_Address	=		"192.168.2.11"		; CM IP address. Set to 127.0.0.1 for localhost (on win)
gS_IP_Address	=		"127.0.0.1"		; CM IP address. 

;_____________________________________________________________________________________________


;_____________________________________________________________________________________________
; GLOBAL VARIABLES
;_____________________________________________________________________________________________
gkReady1		init		0
gkReady2		init		0
gkActNorm1		init		0
gkActNorm2		init		0
gkID			init		1
;_____________________________________________________________________________________________
; GLOBAL FTABLES
; TABLES FOR PARTIKKEL (could these be moved to the local instrument and used with ftgentmp instead?)
;giGainMasks1	ftgen		0, 0, 16, -2, 0, 0, 1
;giGainMasks2	ftgen		0, 0, 16, -2, 0, 0, 1
;giChannelmasks_1	ftgen		0, 0, 16, -2, 0, 0, 0.01
;giChannelmasks_2	ftgen		0, 0, 16, -2, 0, 0, 0.99
;giWaveAmpTab_1	ftgen		0, 0, 16, -2, 0, 0, 0.25, 0.25, 0.25, 0.25, 0
;giWaveAmpTab_2	ftgen		0, 0, 16, -2, 0, 0, 0.25, 0.25, 0.25, 0.25, 0
;; Grain envelope tables
;giSigmoRise 	ftgen	0, 0, 8193, 19, 0.5, 1, 270, 1			; rising sigmoid
;giSigmoFall 	ftgen	0, 0, 8193, 19, 0.5, 1, 90, 1				; falling sigmoid
;giExpFall		ftgen	0, 0, 8193, 5, 1, 8193, 0.00001			; exponential decay
;giTriangleWin 	ftgen	0, 0, 8193, 7, 0, 4096, 1, 4096, 0			; triangular window 
;; Basic waveforms
;giSine		ftgen 0, 0, 8192, 10, 1
;giCosine		ftgen 0, 0, 8192, 9, 1, 1, 180
;
;; TABLES FOR CHOICE OF LONGS
;giChoiceLongs	ftgen		0, 0, 128, -2, 0
;_____________________________________________________________________________________________

;==========================================================================================
;==========================================================================================
;==========================================================================================
; 	INCLUDE UDOs SECTION
;==========================================================================================
;==========================================================================================
;==========================================================================================

; Make UDO for synthesizing OSCadresses to listen to 

; Make UDO that finds the sound files in a folder and returns an array containing sound tables
; Make UDO to assign python strings to csound strings (see CountFilesInFolder.csd)
; Make UDO to get put file lengths of the sound files in a folder into an array
; NB: Have spaces between include lines - else will give error!!!

#include "Inc/Udo/Debounce.udo"

; UDO that lists the names of the folders in a folder, here the set names
#include "Inc/Udo/GetFolderNames.udo"

#include "Inc/Udo/ReadAndSortFiles.udo"

#include "Inc/Udo/Metro_a.udo"
;opcode	Test,	S[],S
;String		xin
;SArray[]	init	2
;SArray[0]	=	"hil"
;SArray[1]	=	"fe!"
;xout	SArray
;endop


;==========================================================================================
;==========================================================================================


;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
; 		Communication with Control Module - request initialization
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		instr		1

; On load; successful start of application

		OSCsend		1 , gS_IP_Address, giME2CM , "/set/ME/loaded", "i", 1

k_init		init		0
kans		OSClisten	giCMserver, "/set/ME/initialize", "i", k_init

kRunning	active	2			; Check if main control instrument is already running

if		kans == 1	&& kRunning ==	0	then
event		"i", 2, 0, $ALWAYSON
elseif	kans == 1	&& kRunning == 1		then; if one is running already, re-initialize
turnoff2	2, 0, 1
turnoff2	10, 0, 1
turnoff2	21, 0, 1			
event		"i", 2, 0, $ALWAYSON
endif

		endin

;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;		Receive OSC-data from motion tracking system, cook/prepare global parameters
;		Manage sound sets 1 and 2 for one and two player modes
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		instr		2
print		p1
; Get path. This will probably be a static path so that it can be set directly in the code
SPath		=		"$Path"
gSetsPath	strcat	SPath, "Sets"

; Get array with names of sets in the /Sets folder
iNumFolders, gSName[]		GetFolderNames	gSetsPath

; Send names of chosen sound banks to MC master control program
String	=		""
index		=		0
ConcatNames:
 String	strcat	String, gSName[index]
 if	index < iNumFolders-1	then
  String	strcat	String, ","
 endif
loop_lt	index, 1, iNumFolders, ConcatNames
;prints	String

; Send concatenated string with soundbank/set names (change to correct IP address)
		OSCsend	1, gS_IP_Address, giME2CM, "/set/player/soundbank/list", "s", String
		OSCsend	1, gS_IP_Address, giME2CM, "/set/zone/soundbank/list", "s", String
	
; Request needed tracking messages (change to correct IP address)
		OSCsend	1, gS_IP_Address, giME2TM, "/set/alphabet/player/activity", "i", 1
		OSCsend	1, gS_IP_Address, giME2TM, "/set/alphabet/player/location", "i", 1
		OSCsend	1, gS_IP_Address, giME2TM, "/set/alphabet/player/position", "i", 1
		OSCsend	1, gS_IP_Address, giME2TM, "/set/alphabet/player/gesture", "i", 1

kSoundSet	init		0

		event_i	"i", 10, 0.01, $ALWAYSON, i(kSoundSet)

if		gkReady1	==	1	then
		OSCsend	gkReady1, gS_IP_Address, giME2CM, "/set/ME/ready", "i", 1
endif


; Receive OSC data from tracking module (for testing at the moment.....)
;???????????????????????????????????????????
gkactNorm1	init		0
gkcenterX	init		0

kans		OSClisten	giTMserver, "/player/0/activity/normal/body", "f", gkactNorm1
kans		OSClisten	giTMserver, "/player/0/centerX", "f", gkcenterX

ametro, kmetro		Metro_a	gkactNorm1*30
aosc		oscil		0.1, 200 + 200 * gkcenterX
outs		ametro, aosc
;???????????????????????????????????????????

		endin

;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;		Manage mappings and sound types within set: particles, longs, rhythms, prize, ambient
;		Instrument activated anew with each set
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		instr		10, 11
Set			=			gSName[p4]				; Read set name from global array
SetSlash		strcat		"\\", Set
SetPath		strcat		gSetsPath, SetSlash
SPrtclePath		strcat		SetPath, "\\Patch_structure\\Sounds\\Layered\\Layer1"
prints	SPrtclePath
SortedFiles[], iNumFiles	ReadAndSortFiles	SPrtclePath, ".aif"
printf_i		"\nStarting set management instrument with set %s containing %i files\n", 1, Set, iNumFiles

; Make tables with sound files, store table numbers in an array
; NB! Will crash if other file types are in the folder (e.g. .asd).
giTabNums[]		init			iNumFiles
indx1			=			0
maketables:
    ifn			ftgentmp		0, 0, 0, 1, SortedFiles[indx1], 0, 0, 0 
    giTabNums[indx1]	=			ifn
loop_lt	indx1, 1, iNumFiles, maketables

if		p1 == 10	&& indx1 == iNumFiles then
gkReady1	init		1
elseif	p1 == 11	&& indx1 == iNumFiles then
gkReady2	init		1
else
gkReady1	=		0
gkReady2	=		0
endif
;------------
; Make triggers to start and stop sound type manage instruments with trigger opcode
; Use fractional instruments to distinguish which person .0= one person only, .1= instrument for person 1, .2, instrument for person 2
; Use conditional statements and turnoff2 (e.g. turnoff2	2.1, 4, 1) to turn instruments off for each person



		endin

;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;		Manage particles instrument instances + map parameters
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		instr		21

; Table that maps activity (index) to frequency - concatenates two curves with different slopes
itabsize	=		4096					; Table size for envelope (linseg fashion)
ithreshpst	=		0.05					; Factor/ratio of initial slope
ithresh	=		int(ithreshpst * itabsize)
ibegslope	=		-1 					; Negative yields convex curve
iendslope	=		2					; Positive yields concave curve
imedfreq	=		12					; Mid-way triggering frequency
imax		=		100					; Maximal triggering frequency

giActMap	ftgentmp	0, 0, itabsize, -16, 0, ithresh, ibegslope, imedfreq, itabsize-ithresh, iendslope, imax

kTrigFreq1	table	  	gkActNorm1, giActMap, 1		; Read correct frequency of trigger according to table
kTrigFreq2	table	  	gkActNorm2, giActMap, 1		; Read correct frequency of trigger according to table


; Trigger/sync for partikkel instrument
gaSyncTrig_ID1, gkTrig1		Metro_a	kTrigFreq1	; Trigger grains ID 0 and 1 with partikkel instrument
gaSyncTrig_ID2, gkTrig2		Metro_a	kTrigFreq2	; Trigger grains ID 2 with partikkel instrument

		endin

;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;		Manage longs instrument instances
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		instr		22
; Make tables from sound files in Set/Sounds/Longs folder using directory and ftgentmp opcodes
; Write sound file duration and table numbers to two arrays holding these values

		endin

;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;		Manage rhythms instrument instances
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		instr		23
/*
- Read rhythm file from set (imported from MIDI)
- Since it presupposes the particles instrument, it can run with the same ftables created by the
 instrument managing particle instances (21)
- 
- Set up metro with variable tempo to trigger individual sounds from rhythm playback instrument
*/
		endin

;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;		Manage prize sounds instrument instances
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		instr		24

		endin

;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;		Manage ambient sounds instrument instances
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		instr		25

		endin



;**************************************************************************************************
;		Play particles instrument instances
;**************************************************************************************************
		instr		101

; Source waveforms
kSndLayer1		= 		101		; Layer 1 (use waveforms for layer 1 made with instr 10)
kSndLayer2		= 		102		; Layer 2
kSndLayer3		= 		103		; Layer 3
kSndLayer4		= 		104		; Layer 4

; Use included file with basic settings'
;#include "Inc/Instr/partikkel_basic_settings.inc"

kgrainrate		=		0		; should be set to zero to receive sync pulses from the trigger instrument

if			gkID == 1	|| gkID == 0 	then
async			=		gaSyncTrig_ID1
elseif		gkID == 2	then
async			=		gaSyncTrig_ID2
endif

; Temporarly setting these variables for testing
kamp			=			ampdb(-7)
ktranspose		=			1

;a1,a2,a3,a4,a5,a6,a7,a8	partikkel \					;
;		  kgrainrate, \							; grains per second			*
;		  kdistribution, idisttab, async, \				; synchronous/asynchronous		
;		  kenv2amt, ienv2tab, ienv_attack, ienv_decay, \	; grain envelope (advanced)		
;		  ksustain_amount, ka_d_ratio, kduration, \		; grain envelope 			*
;		  kamp, \								; amp					*
;		  igainmasks, \							; gain masks (advanced)			
;		  ktranspose, \							; grain transpose (playback frequency)	*
;		  ksweepshape, iwavfreqstarttab, iwavfreqendtab, \	; grain pith sweeps (advanced)		
;		  awavfm, ifmamptab, ifmenv, \				; grain pitch FM (advanced)		
;		  icosine, kTrainCps, knumpartials, kchroma, \		; trainlets				
;		  ichannelmasks, \ 						; channel mask (advanced)
;		  krandommask, \							; random masking of single grains	*
;		  kSndLayer1, kSndLayer2, kSndLayer3, kSndLayer4, \	; set source waveforms			
;		  iwaveamptab, \							; mix source waveforms
;		  asamplepos1, asamplepos2, asamplepos3, asamplepos4, \	; read position for source waves
;              kwavekey1, kwavekey2, kwavekey3, kwavekey4, \		; individual transpose for each source
;		  imax_grains							; system parameter (advanced)

		endin

;**************************************************************************************************
;		Play longs instrument instances
;**************************************************************************************************
		instr		102

		endin

;**************************************************************************************************
;		Play rhythms instrument instances
;**************************************************************************************************
		instr		103

		endin

;**************************************************************************************************
;		Play prize sounds instrument instances
;**************************************************************************************************
		instr		104

; Prize sounds should be stereo

		endin

;**************************************************************************************************
;		Play ambient sounds instrument instances
;**************************************************************************************************
		instr		105

; Ambient sounds should be stereo

		endin

;**************************************************************************************************
;		Effect instruments
;**************************************************************************************************
		instr		200
/*
- Have both effect code and mappings in one single instrument that is included on trig. This could
be done with compileorc opcode.
- Several effects can be set up in a series using same input and output variables
- Flexible mapping could be made with a combination of temporary ftables and an array holding the tracking variables.
*/
		endin


;**************************************************************************************************
;		Mix and output, clear global variables
;**************************************************************************************************
		instr		1000

		endin

</CsInstruments>


<CsScore>
i 1		0	72000

</CsScore>


</CsoundSynthesizer>
