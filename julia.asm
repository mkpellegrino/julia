	;    (C) 2019 - Michael K. Pellegrino
	; 64bit Assembler Library Written for MacOSX
	;
	; compile with:
	; nasm -f macho64 -g -DDEBUG -l ./bin/sandbox_a.lst sandbox.asm -o ./bin/sandbox_a.o
	;
	; link with:
	; g++ -g -DDEBUG ./bin/another_object_file.o ./bin/sandbox_a.o -o ./bin/executable_program
	;
	; in c++ code:
	; extern "C" int sandbox(void* pBuffer); // returns how many swaps the function had to make in order to sort
[bits 64]
section .data
%ifdef DEBUG
	tmp dq 0
%endif
	A dq 0
	B dq 0
	C dq 0
	D dq 0
	ASMBS dq 0			   ; A^2 - B^2
	TAB dq 0			   ; 2AB
	TWO dq __float64__(2.0)		   ; the constant 2
	NIL dq __float64__(0.0000000002) ;something close to zero
	FIVE dq __float64__(5.000000000)   ; 5
	CTR db 0x20		           ; Counter (Number of Iterations)
	
section .text
global _julia
global _set_n

_julia:
	db 0x53, 0x51, 0x52, 0x41, 0x51, 0x41, 0x52, 0x41, 0x53, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57 ; Push All Registers except rax, rdi, and rsi
;;;============================================
	finit
	movq rax, xmm0		; AR
	movq rbx, xmm1		; BC
	movq r9, xmm2		; CR
	movq r10, xmm3		; CC

	mov rdi, A
	mov QWORD [rdi], rax	
	mov rdi, B
	mov QWORD [rdi], rbx
	mov rdi, C
	mov QWORD [rdi], r9
	mov rdi, D
	mov QWORD [rdi], r10

	xor ecx,ecx
	mov rdi, CTR		; set the counter (to CUTOFF)
	mov cl, BYTE [rdi]

	xor eax, eax
	
looptop:
	mov rdi, C
	fld QWORD [rdi]		; st0=CReal
	mov rdi, B
	fld QWORD [rdi]		; st0=B, st1=CReal
	fld st0			; st0=B, st1=B, st2=CReal
	fmulp			; st0=B^2, st1=CReal
	mov rdi, A
	fld QWORD [rdi]		; st0=A, st1=B^2, st2=CReal
	fld st0			; st0=A, st1=A, st2=B^2, st3=CReal
	fmulp			; st0=A^2, st1=B^2, st2=CReal
	fsubp			; st0=A^2-B^2, st1=CReal
	faddp			; st0=A^2-B^2+CReal

	

	mov rdi, ASMBS		; Save A^2-B^2+CReal in A^2-B^2
	fstp QWORD [rdi]	; stack is empty

	mov rdi, D		; CC
	fld QWORD [rdi]		; st0=CComplex
	mov rdi, A		; AR
	fld QWORD [rdi]		; st0=A, st1=CComplex
	
	mov rdx, ASMBS		; 
	fld QWORD [rdx]		; st0=A^2-B^2+CReal, st1=A, st2=CComplex
	fstp QWORD [rdi]	; st0=A, st1=CComplex
	
	mov rdi, B
	fld QWORD [rdi]		; st0=B, st1=A, st2=CComplex
	
	mov rdi, TWO
	fld QWORD [rdi]		; st0=2, st1=B, st2=A, st3=CComplex
	fmulp			; st0=2B, st1=A, st2=CComplex
	fmulp			; st0=2BA, st1=CComplex
	faddp			; st0=2AB+CComplex


	mov rdi, TAB		
	fst QWORD [rdi]		; Save 2AB+CComplex in TAB
	mov rdi, B		; -> B
	fstp QWORD [rdi]	; Save 2AB+CComplex in TAB 

;%ifdef DEBUG			; this will help verify that the correct values
	;push rdi		; are being calculated in each iteration
	;push rbx
	;mov rdi, ASMBS
	;mov rbx, [rdi]
	;mov rdi, TAB
	;mov rbx, [rdi]
	;pop rbx
	;pop rdi
	;%endif

	mov rdi, A
	mov QWORD rbx, [rdi]	; and if it is, the function returns 0
	rol rbx, 0x10
	cmp rbx, 0xFFF8
	je retz

	mov rdi, B
	mov QWORD rbx, [rdi]	 ; and if it is, the function returns 0
	rol rbx, 0x10
	cmp rbx, 0xFFF8
	je retz


	dec rcx
	jnz looptop		; END OF LOOP
;;; END OF LOOP
;%ifdef DEBUG
	;mov rdi, ASMBS	       ; for debugging only ASMBS+C -> 0x100001018
	;mov rbx, [rdi]
	;rol rbx, 0x10
	;fld QWORD [rdi]
	;mov rdi, tmp
	;fstp QWORD [rdi] 	; for debugging only ASMBS+C -> 0x100001018
;%endif
;%ifdef DEBUG
	;mov rdi, TAB		; for debugging only 2AB+C -> 0x100001018
	;mov rbx, [rdi]
	;rol rbx, 0x10
	;fld QWORD [rdi]
	;mov rdi, tmp
	;fstp QWORD [rdi] 	; for debugging only 2AB+C -> 0x100001018
;%endif
	
	mov rdi, A
	;mov QWORD rbx, [rdi]	; and if it is, the function returns 0
	;rol rbx, 0x10
	;cmp rbx, 0xFFF8
	;je retz

	fld QWORD [rdi]
	fld st0
	fmulp
	
	mov rdi, B
	;mov QWORD rbx, [rdi]	 ; and if it is, the function returns 0
	;rol rbx, 0x10
	;cmp rbx, 0xFFF8
	;je retz

	fld QWORD [rdi]
	fld st0
	fmulp

	faddp
	fsqrt
	mov rdi, NIL 		; 2^(-10)
	fld QWORD [rdi]
	xor eax,eax
	
	
	;fxch
	
	fucomp 		; compare the last term with NIL
	jb retz		; if st0 > st1  (NIL > value)
	
	mov rdi, FIVE
	fld QWORD [rdi]
	fucomp 
	
	ja retz			; if st0 < st1  (5<value)
	mov rax, 0xFF
retz:
;;;============================================	
	db 0x41, 0x5F, 0x41, 0x5E, 0x41, 0x5D, 0x41, 0x5C, 0x41, 0x5B, 0x41, 0x5A, 0x41, 0x59, 0x5A, 0x59, 0x5B ; Pop All Registers except rax, rdi, and rsi
	
	ret


_set_n:
	
	mov rax, rdi
	mov rdi, CTR
	mov BYTE [rdi], al
	ret
_end_of_code:
