; Opcode to prevent retriggering of a trigger within a specified debounce interval
opcode	Debounce, k, ki
kInval, iDebounceTime		xin
kDebounce	 	init		0

kLastval		init		0

kVal_chg		changed	kInval
kVal_new		=		kVal_chg == 1 ? 1: 0

kOutVal		=		kDebounce > 0? kLastval: kInval				; Only let kOutVal through if debounce > 0

if			kDebounce > 0	then
kDebounce		-=		iDebounceTime / kr
endif


if			kDebounce <= 0	&& kVal_new	== 1	then
kDebounce		=		iDebounceTime						; Start countdown when new trigger is received
kLastval		=		kInval
endif

xout			kOutVal
endop