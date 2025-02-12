BEGIN 1000
begin:  MOV ax, a
	MOV bx, b 
	ADD ax, bx
	MOV c, ax
	HLT		;стоп
a:	DB 15
b:	DB 7
c:	DW 0
END begin
