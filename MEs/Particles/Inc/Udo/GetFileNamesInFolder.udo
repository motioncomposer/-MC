/*
GetFolderNames - query folder names in a folder and count them

DESCRIPTION
Queries the names of folders in a folder and counts them.
Writes folder names to a string array

SYNTAX
iNumFiles, SNamesArray[]	GetFolderNames	SFolder

INITIALIZATION
Input: Full folder path
Output: Number of folders (exluding _Tmp and .DS), Names of folders (array)

PERFORMANCE
Works only at init time

CREDITS
Andreas Bergsland 2017
*/
opcode GetFileNamesInFolder,iS[], S
SFolder		xin					; String with folder to query
;kIndexFile	=		int(kIndexFile)
; Define ascii 37-126
Sascii	=		"%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
SOutFileNames[]	fillarray	"a", "b", "c"
iNumFiles		lenarray	SOutFileNames
; 1. Pass input string to python interpreter
				;input string
ilen       	strlen     	SFolder		; its length
iArray[]	init		ilen			; Initiate array for string

indx1		=		0

loop:                 					;for all characters in input string:
ichr       strchar    	SFolder, indx1 		;get its ascii code number
iArray[indx1]=		ichr
           loop_lt    	indx1, 1, ilen, loop 	;see comment for 'loop:'

pyruni		{{
import	os
Folder 	= ''''''
FolderNames = [] 
def   getValidFiles(Path):
	InDirList = os.listdir(Path)
	print InDirList
	OutDirList = []
	for i in InDirList:
		if	os.path.isdir(Path + "/"+ i) == True and i != '_Tmp':
			OutDirList.append(i)
	NumFolders = float(len(OutDirList))
	return OutDirList, NumFolders
}}

; Convert from ascii to string and concatenate
indx2		=		0
loop2:
pyassigni	"FolderAscii", iArray[indx2]
pyruni	"Folder = Folder + chr(int(FolderAscii))"
		loop_lt, indx2, 1, ilen, loop2

pyruni	{{
FolderList, NumFolders = 	getValidFiles(Folder)
print	FolderList, NumFolders
}}

;;; The number of files in the folder
;iNumFolders	pyevali	"NumFolders"
;SOutFolders[] init		iNumFolders
;
;; Loop through the folder list
;indx3		=		0
;loop3:
;pyassigni	"g", indx3
;pyruni	{{
;folder = FolderList[int(g)]
;folderLen = float(len(folder))
;}}
;ifolderLen	pyevali	"folderLen"
;; and for each folder name convert to ascii
;iasciindx2 = 0
;SFolderName	strcpy		""	; Empty string
;asciiloop2:
;pyassigni	"h", iasciindx2
;pyruni	{{
;asciiOut	=	float(ord(folder[int(h)]))
;}}
;iasciiOut	pyevali	"asciiOut"
;; Bug in strsub causes indices equal to or higher than 3 to extract wrong substring? Using worksaround to solve this here
;istart	=		iasciindx2 < 3? iasciiOut - 36: iasciiOut - 37
;iend		=		iasciindx2 < 3? iasciiOut - 37 : iasciiOut - 38
;; -------------------------
;SOut		strsub	Sascii, istart, iend
;SFolderName	strcat	SFolderName , SOut
;		loop_lt iasciindx2, 1, ifolderLen, asciiloop2
;SOutFolders[indx3]	=	SFolderName
;	loop_lt	indx3, 1, iNumFolders, loop3

xout	iNumFiles, SOutFileNames

endop
