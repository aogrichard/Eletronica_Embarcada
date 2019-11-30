//Ponto de Controle 4
//Rotina referente a função assemble do codigo
//versao teste para validar a integracao


	.cdecls C,NOLIST, "msp430.h"   ; Processor specific definitions
      
      
;============================================================================
; scan
;============================================================================
	.global scan               ; Declare symbol to be exported
	.sect ".text"                  ; Code is relocatable
scan:   .asmfunc 

      scan():
c3ce:   425F 0242           MOV.B   &col,R15
c3d2:   5F0F                RLA.W   R15
c3d4:   503F 0220           ADD.W   #0x0220,R15
112       uint8_t p1=*pout, p2=*(pout+1);
c3d8:   4F6E                MOV.B   @R15,R14
c3da:   4F5F 0001           MOV.B   0x0001(R15),R15
114       col++;
c3de:   53D2 0242           INC.B   &col
115       col &= 0x0f;
c3e2:   F0F2 000F 0242      AND.B   #0x000f,&col
117       P1DIR = P2DIR = 0;
c3e8:   43C2 002A           CLR.B   &Port_1_2_P2DIR
c3ec:   43C2 0022           CLR.B   &Port_1_2_P1DIR
118       P1OUT = P2OUT = 0;
c3f0:   43C2 0029           CLR.B   &Port_1_2_P2OUT
c3f4:   43C2 0021           CLR.B   &Port_1_2_P1OUT
119       if (!col) return;
c3f8:   93C2 0242           TST.B   &col
c3fc:   2417                JEQ     ($C$L2)
120       P1DIR = p1|EQ_RESET_PIN|EQ_STROBE_PIN;
c3fe:   4E0D                MOV.W   R14,R13
c400:   D07D 0006           BIS.B   #0x0006,R13
c404:   4DC2 0022           MOV.B   R13,&Port_1_2_P1DIR
121       P2DIR = p2;
c408:   4FC2 002A           MOV.B   R15,&Port_1_2_P2DIR
122       p1 &= p1Lin;
c40c:   F07E 0038           AND.B   #0x0038,R14
123       p2 &= p2Lin;
c410:   F07F 0007           AND.B   #0x0007,R15
124       if (!(col&0x08)) {
c414:   B2F2 0242           BIT.B   #8,&col
c418:   2003                JNE     ($C$L1)
125           p1 ^= 0xf8;
c41a:   E07E 00F8           XOR.B   #0x00f8,R14
126           p2 ^= 0xff;
c41e:   E37F                INV.B   R15
129       P1OUT = p1;
      $C$L1:
c420:   F07E 00F8           AND.B   #0x00f8,R14
c424:   4EC2 0021           MOV.B   R14,&Port_1_2_P1OUT
130       P2OUT = p2;
c428:   4FC2 0029           MOV.B   R15,&Port_1_2_P2OUT
132   }
      $C$L2:
c42c:   4130                RET

	.endasmfunc

	.end
