//Ponto de Controle 4
//Arquivo referente ao teste efetuado na inicializacao do timer_a a fim de verificar e validar a integracao do assemble ao codigo em c.

	.cdecls C,NOLIST,"msp430.h"

	.global timer_a               ; Declare symbol to be exported
	.sect ".text"                  ; Code is relocatable
timer_a:   .asmfunc

;Timer0_A0_iSR():
	PUSH    R15

;eqCtrl = (eqCounter >> 2) & 0x07;
	MOV.B   &eqCounter,R15
	RRA     R15
	RRA     R15
	AND.B   #0x0007,R15
	MOV.B   R15,&eqCtrl
	TST.B   &eqCtrl
	JNE     ($C$L34)

;if (eqCounter == 0) {
	TST.B   &eqCounter
	JNE     ($C$L37)

; P1OUT |= EQ_RESET_PIN;
	BIS.B   #2,&Port_1_2_P1OUT

;P1OUT &= ~EQ_RESET_PIN;
	BIC.B   #2,&Port_1_2_P1OUT

;eqCounter += 4;
	ADD.B   #4,&eqCounter
	JMP     ($C$L37)

; else if ((eqCounter & 0x03) == 3) 

$C$L34:
	MOV.B   &eqCounter,R15
	AND.W   #0x0003,R15
	CMP.W   #0x0003,R15
	JEQ     ($C$L36)

; else if (eqCounter & 0x01) 
	BIT.B   #1,&eqCounter
	JNE     ($C$L35)

; else if (eqCounter & 0x02) 
	BIT.B   #2,&eqCounter
	JEQ     ($C$L37)

; P1OUT &= ~EQ_STROBE_PIN;

	BIC.B   #4,&Port_1_2_P1OUT
	JMP     ($C$L37)

;P1OUT |= EQ_STROBE_PIN;
$C$L35:
	BIS.B   #4,&Port_1_2_P1OUT

;else if (eqCounter & 0x02) {
	JMP     ($C$L37)

;ADC10CTL0 |= ENC + ADC10SC;

$C$L36:
	BIS.W   #0x0003,&ADC10_ADC10CTL0

;eqCounter++;
$C$L37:
	INC.B   &eqCounter

;timerCounter++;
	INC.B   &timerCounter

	.endasmfunc
	.end
