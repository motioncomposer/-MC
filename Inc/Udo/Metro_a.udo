/*
Metro_a - a-rate metronomeUDO 

DESCRIPTION
Creates a one-sample metronome at desired frequency
SYNTAX
apulse	metro_a	kcps, initphase

INITIALIZATION
Phase (optional)

PERFORMANCE
Metronome frequency

CREDITS
by Andreas Bergsland, 2013
*/

opcode	Metro_a, ak, ko
kcps, initphase	xin
kmetro	metro		kcps
ametro	upsamp	kmetro
adel		delay1	ametro
apulse	=		(ametro - adel)
apulse	=		0.5 * (apulse + abs(apulse))
xout		apulse, kmetro
endop