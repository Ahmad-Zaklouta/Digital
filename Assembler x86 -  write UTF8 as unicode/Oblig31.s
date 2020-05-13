	.extern	fread, fwrite
	
	.data
c:		.byte 	0
status:	.long 	0
uctxt:	.long 	0
filen:	.long 	0
Tmp:	.byte 	0

	.text
	.globl		readbyte
 # Navn:		readbyte
 # Synopsis:	Leser en byte fra en binærfil.
 # C-signatur: 	int readbyte (FILE *f)
 # Registre:
	
readbyte:
	pushl	%ebp			# Standard funksjonsstart
	movl	%esp,%ebp		
	
	movl	8(%ebp), %eax	#Pusher filen på stakken
	pushl	%eax
	
	movl	$1, %eax		
	pushl	%eax
	
	movl	$1, %eax	
	pushl	%eax
	
	leal 	c, %eax			#Pusher adressen til bytet på stakk
	pushl 	%eax
	
	call	fread
	
	movl	%eax, status	
	addl	$0,status		#Setter nullflagget
	
	jz 		ferdig			#Skjekker om filen er tom ved å se på nullflagget
	
	movl	c, %eax			#Flytter bytet til %eax for retur
	jmp 	rb_x

ferdig:						#Blir kalt om filen er tom
	movl	$-1, %eax		#Flytter -1 til %eax siden filen er tom
	jmp 	rb_x

rb_x:	
	addl	$16, %esp		#Flytter %esp til riktig posisjon på stakk
	popl	%ebp			# Standard
	ret						# retur.



	.globl		readutf8char
 # Navn:		readutf8char
 # Synopsis:	Leser et Unicode-tegn fra en binærfil.
 # C-signatur: 	long readutf8char (FILE *f)
 # Registre:
	
readutf8char:
	pushl	%ebp				# Standard funksjonsstart
	movl	%esp,%ebp	 		
	movl	8(%ebp), %edx
	movl	%edx,filen 			#Flytter filpekeren til fil-variabelen. 
	
	pushl	filen				#Flytter filen til stakken	
	call 	readbyte			#Henter ut en byte
	movl	%eax,uctxt			#Lagrer bytet i uctxt
	popl	%eax
	movl	uctxt,%eax
	cmpl	$-1,%eax			#Sjekker om filen er tom
	jz		IkkeFlerTegn

														
	movl	uctxt,%eax			#Sjekker hvor mange byte som tegnet er 
	andl	$0xF8,%eax			#representert med.	
	cmpl	$0xF0,%eax
	jz		FireBit
	
	movl	uctxt,%eax
	andl	$0xF0,%eax
	cmpl	$0xE0,%eax
	jz		TreBit
	
	movl	uctxt,%eax
	andl	$0xE0,%eax
	cmpl	$0xC0,%eax
	jz		ToBit
		
	movl	uctxt,%eax
	andl	$0x80,%eax
	cmpl	$0,%eax
	jz		EttBit

	jmp		read_ret
	
EttBit:						#Returnerer det ene bytet som ble hentet.
	xorl	%eax,%eax
	movl	uctxt,%eax
	andl	$0x7F,%eax
	jmp read_ret

ToBit:						#Returnerer to byte fra filen
	xorl	%eax,%eax		#Nuller ut registerene som blir brukt
	xorl	%edx,%edx
	movl	uctxt,%edx		#Flytter bittet hentet fra filen inn i et 
	andl	$0x1F,%edx		#Masker ut så jeg får kun de bitene som jeg vil.
	movb	%dl,Tmp			#Lagrer masken i en variabel for senere bruk. 
	pushl	filen			#Pusher filen.
	call	readbyte		#Henter ut en byte til.
	andl	$0x3F,%eax		#Masker ut de bitene jeg vil ha
	movb	Tmp,%dl			#Flytter variabelen tilbake til registeret %edx
	sall	$6,%edx			#Shifter %edx
	orl	%edx,%eax		#Setter alle bitene sammen
	popl	%ecx
	jmp read_ret

TreBit:						#Returnerer tre byte fra filen
	xorl	%eax,%eax		#Nuller ut registerene som blir brukt
	xorl	%edx,%edx
	movl	uctxt,%edx		#Flytter bittet hentet fra filen inn i et register
	andl	$0xF,%edx		#Masker ut så jeg får kun de bitene som jeg vil.
	movb	%dl,Tmp			#Lagrer masken i en variabel for senere bruk.
	 
	pushl	filen			#Pusher filen.
	call	readbyte		#Henter ut en byte til.
	popl	%ecx			#Popper stakken
	andl	$0x3F,%eax		#Masker ut de bitene jeg vil ha
	movb	Tmp,%dl			#Flytter variabelen tilbake til registeret %edx
	sall	$6,%edx			#Shifter %edx
	orl		%edx,%eax		#Setter alle bitene sammen
	movl	%eax,uctxt		#Flytter den nye byten inn i en variabel
	
	pushl	filen			#Pusher filen
	call	readbyte	 	#Henter ut en byte til.
	popl	%ecx			#Popper stakken
	andl	$0x3F,%eax		#Masker ut de bitene jeg vil ha
	movl	uctxt,%edx		#Flytter variabelen tilbake til registeret %edx
	sall	$6,%edx			#Shifter %edx
	orl		%edx,%eax		#Setter alle bitene sammen
	jmp read_ret

FireBit:					#Returnerer fire byte fra filen
	xorl	%eax,%eax		#Nuller ut registerene som blir brukt
	xorl	%edx,%edx
	movl	uctxt,%edx		#Flytter bittet hentet fra filen inn i et register
	andl	$0x7,%edx		#Masker ut så jeg får kun de bitene som jeg vil.
	movb	%dl,Tmp			#Lagrer masken i en variabel for senere bruk.
	
	pushl	filen			#Pusher filen.
	call	readbyte		#Henter ut en byte til.
	popl	%ecx			#Popper stakken
	andl	$0x3F,%eax		#Masker ut de bitene jeg vil ha
	movb	Tmp,%dl			#Flytter variabelen tilbake til registeret %edx
	sall	$6,%edx			#Shifter %edx
	orl		%edx,%eax		#Setter alle bitene sammen
	movl	%eax,uctxt		#Flytter den nye byten inn i en variabel
	
	pushl	filen			#Pusher filen
	call	readbyte		#Henter ut en byte til.
	popl	%ecx			#Popper stakken
	andl	$0x3F,%eax		#Masker ut de bitene jeg vil ha
	movl	uctxt,%edx		#Flytter variabelen tilbake til registeret %edx
	sall	$6,%edx			#Shifter %edx
	orl		%edx,%eax		#Setter alle bitene sammen
	movl	%eax,uctxt
	
	pushl	filen			#Pusher filen
	call	readbyte		#Henter ut en byte til.
	popl	%ecx			#Popper stakken
	andl	$0x3F,%eax		#Masker ut de bitene jeg vil ha
	movl	uctxt,%edx		#Flytter variabelen tilbake til registeret %edx
	sall	$6,%edx			#Shifter %edx
	orl		%edx,%eax		#Setter alle bitene sammen

	jmp read_ret

IkkeFlerTegn:				#Blir kalt om det ikke er flere tegn
	movl	$-1,%eax		#Flytter konstanten -1 inn i %eax
	jmp 	read_ret		

read_ret:
	popl	%ebp			# Standard
	ret						# retur.


	.globl		writebyte
 # Navn:		writebyte
 # Synopsis:	Skriver en byte til en binærfil.
 # C-signatur: 	void writebyte (FILE *f, unsigned char b)
 # Registre:
	
writebyte:
	pushl	%ebp			# Standard funksjonsstart
	movl	%esp,%ebp		#
	leal 	12(%ebp),%edx	#Flytter unicodetegnet til en variabel
	movl	%edx, c
	movl	8(%ebp),%ecx	#Flytter filen til en variabel
	movl	%ecx,filen
	
	movl	filen,%eax		#Setter inn filadressen inn på stakk
	pushl	%eax
	
	movl	$1, %eax		#Setter lengden av byten inn på stakk
	pushl	%eax
	
	movl	$1, %eax		#Setter inn antall byte på stakk
	pushl 	%eax
	
	movl	c,%eax			#Setter inn byten på stakk
	pushl	%eax

	call 	fwrite			#Skriver til selve filen
	
	popl 	%eax
	popl 	%eax	
	popl 	%eax
	popl 	%eax

	popl	%ebp			# Standard
	ret						# retur.
	


	.globl		writeutf8char
 # Navn:		writeutf8char
 # Synopsis:	Skriver et tegn kodet som UTF-8 til en binærfil.
 # C-signatur: 	void writeutf8char (FILE *f, unsigned long u)
 # Registre:

writeutf8char:
	pushl	%ebp			# Standard funksjonsstart
	movl	%esp,%ebp		#
	movl 	12(%ebp),%edx	#Flytter unicodetegnet til en variabel
	movl	%edx, uctxt
	movl	8(%ebp),%ecx	#Flytter filen til en variabel
	movl	%ecx,filen
	
	subl	$0x7F,%edx		#Skjekker hvor mange bit i UTF-8 som trengs.
	js		bit1			#Og sender byten til riktig funksjon.
	jz		bit1
	
	movl	uctxt,%edx	
	subl	$0x7FF,%edx
	js		bit2	
	jz		bit2	
	
	movl	uctxt,%edx
	subl	$0xFFFF,%edx
	js		bit3	
	jz		bit3
	
	movl	uctxt,%edx
	subl	$0x10FFFF,%edx
	js		bit4	
	jz		bit4
		
	jmp wu8_x

bit4:							#Lager 4 bit UTF-8
	xorl 	%eax, %eax			#Nuller ut registerene
	xorl 	%ecx, %ecx
	xorl 	%edx, %edx
	
	movl	uctxt,%eax
	shrl	$18,%eax
	andl	$0x7,%eax			#Setter inn det første bytet
	orl		$0xF0,%eax
	pushl	%eax
	pushl	filen
	call 	writebyte
	popl	%eax
	popl	%eax
	
	xorl 	%eax, %eax			#Nuller ut registerene
	xorl 	%ecx, %ecx
	xorl 	%edx, %edx
	movl	uctxt,%eax
	shrl	$12,%eax
	andl	$0x3F,%eax			#Setter inn det andre bytet
	orl		$0x80,%eax
	pushl	%eax
	pushl	filen
	call 	writebyte
	popl	%eax
	popl	%eax

	xorl 	%eax, %eax			#Nuller ut registerene
	xorl 	%ecx, %ecx
	xorl 	%edx, %edx
	movl	uctxt,%eax
	shrl	$6,%eax
	andl	$0x3F,%eax			#Setter inn det tredje bytet
	orl		$0x80,%eax
	pushl	%eax
	pushl	filen
	call 	writebyte
	popl	%eax
	popl	%eax
	
	xorl 	%eax, %eax			#Nuller ut registerene
	xorl 	%ecx, %ecx
	xorl 	%edx, %edx
	movl	uctxt,%eax
	andl	$0x3F,%eax			#Setter inn det siste bytet
	orl		$0x80,%eax	
	pushl	%eax
	pushl	filen
	call 	writebyte
	popl	%eax
	popl	%eax

	jmp wu8_x

bit3:							#Lager 3 bit UTF-8	
	xorl 	%eax, %eax			#Nuller ut registerene
	xorl 	%ecx, %ecx
	xorl 	%edx, %edx
	movl	uctxt,%eax
	shrl	$12,%eax
	andl	$0xF,%eax			#Setter inn det første bytet
	orl		$0xE0,%eax
	pushl	%eax
	pushl	filen
	call 	writebyte
	popl	%eax
	popl	%eax

	xorl 	%eax, %eax			#Nuller ut registerene
	xorl 	%ecx, %ecx
	xorl 	%edx, %edx
	movl	uctxt,%eax
	shrl	$6,%eax
	andl	$0x3F,%eax			#Setter inn det andre bytet
	orl		$0x80,%eax
	pushl	%eax
	pushl	filen
	call 	writebyte
	popl	%eax
	popl	%eax
	
	xorl 	%eax, %eax			#Nuller ut registerene
	xorl 	%ecx, %ecx
	xorl 	%edx, %edx
	movl	uctxt,%eax
	andl	$0x3F,%eax			#Setter inn det siste bytet
	orl		$0x80,%eax	
	pushl	%eax
	pushl	filen
	call 	writebyte
	popl	%eax
	popl	%eax

	jmp wu8_x

bit2:							#Lager 2 bit UTF-8
	xorl 	%eax, %eax			#Nuller ut registerene
	xorl 	%ecx, %ecx
	xorl 	%edx, %edx
	movl	uctxt,%eax
	shrl	$6,%eax
	andl	$0x1F,%eax			#Setter inn det første bytet
	orl		$0xC0,%eax
	pushl	%eax
	pushl	filen
	call 	writebyte
	popl	%eax
	popl	%eax

	xorl 	%eax, %eax			#Nuller ut registerene
	xorl 	%ecx, %ecx
	xorl 	%edx, %edx
	movl	uctxt,%eax	
	andl	$0x3F,%eax			#Setter inn det siste bytet
	orl		$0x80,%eax
	pushl	%eax
	pushl	filen
	call 	writebyte
	popl	%eax
	popl	%eax
	jmp wu8_x

bit1:							#Lager 1 bit UTF-8
	xorl 	%eax, %eax
	movl	uctxt,%eax	
	andl	$0x7F,%eax
	pushl	%eax
	pushl	filen				#Sender bytet inn i filen.
	call 	writebyte
	popl	%eax
	popl	%eax
	jmp wu8_x
	
wu8_x:
	popl	%ebp				# Standard
	ret							# retur.
