opcode ReadAndSortFiles, S[]i, SS

SPath, SExt		xin
iPathLen		strlen	SPath
SFiles[]		directory	SPath, SExt
iArrLen		=		lenarray(SFiles)
; Get number part of file name and put it in an array as integers------------
igetNumber[]	init		iArrLen
indx1			=		0
loop1:
ilen			strlen	SFiles[indx1]				; Get length of path + file name
Sordinal		strsub	SFiles[indx1], iPathLen+1, ilen-4	; Extract file name string minus path and extension
;prints		SFiles[indx1]
;prints		"\n"
;prints		Sordinal
;prints	"\n"
iNumber		strtol	Sordinal
igetNumber[indx1]	=		iNumber					; Convert to integer
loop_lt	indx1, 1, iArrLen, loop1
;------------


; Sort files with number array as key------------
SortedFiles[]	init		iArrLen
iMax			maxarray	igetNumber
indx2			=		0
loop2:
iMin, iMinIndx	minarray	igetNumber
SortedFiles[indx2]	=	SFiles[iMinIndx]
igetNumber[iMinIndx]		=	iMax + 1000
loop_lt	indx2, 1, iArrLen, loop2
;------------
xout       SFiles, iArrLen;SortedFiles, iArrLen
endop
