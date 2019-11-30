//Ponto de Controle 4
// Codigo referente as etapas de desenvolvimento para o PC4

#include <msp430.h>
#include <stdint.h>

//extern void timer_a(void);
//extern uint8_t scan(uint8_t mask);        /* Function Prototype for asm function */
//extern void scan(char mask);

const uint8_t lin_col[] = {
    //  Lin     col
    ((0+3)<<4) | (8+3),
    ((0+4)<<4) | (8+4),
    ((0+5)<<4) | (8+5),
    ((8+0)<<4) | (0+6),
    ((8+1)<<4) | (0+7),
    ((8+2)<<4) | (8+7),
    ((0+0)<<4) | (8+6),
    ((0+0)<<4) | (0+2),
};

const uint8_t p1Lin = BIT3|BIT4|BIT5;
const uint8_t p2Lin = BIT0|BIT1|BIT2;

const uint8_t p1cols = BIT6|BIT7;
const uint8_t p2cols = BIT3|BIT4|BIT5|BIT6|BIT7;

volatile uint8_t marca=0, clicks=0;
volatile uint8_t col=0;

uint8_t saida[8*2*2];      // 8 columns of p1+p2 values times 2 sets
uint8_t nivel[] = { 0, 0, 0, 0, 0, 0, 0, 0, }; // 0..11
uint8_t sAtual=0;
uint8_t amostra[4][8];

uint8_t pico[]  = { 0, 0, 0, 0, 0, 0, 0, 0, };
uint8_t saida_pico[]  = { 0, 0, 0, 0, 0, 0, 0, 0, };
uint8_t recuo[]  = { 0, 0, 0, 0, 0, 0, 0, 0, };

#define EQ_DC_IN_PIN BIT0
#define EQ_RESET_PIN BIT1
#define EQ_STROBE_PIN BIT2

uint8_t modo = 0;

#define MODO_PONTOS    BIT0
#define MODO_PICOS   BIT1
#define MODO_2PONTOS   BIT2
#define MODO_AMOSTRA BIT7

//________________________________________________________________________________
void load() {
    uint8_t c=0, r=0;
    for (c=0;c<8;c++) {
        uint8_t c_val = lin_col[c] & 0x0f;
        uint8_t *pout = saida + (c<<1);

        *pout = *(pout+1) = 0;
        if (c_val & 0x08)
            *(pout+1) = 1<<(c_val&0x07);
        else
            *pout = 1<<c_val;
        *(pout+16) = *pout;
        *(pout+17) = *(pout+1);

        if (nivel[c] > 12)
            nivel[c] = 12;

        r = nivel[c];

        uint8_t vai = 0;
        if (modo & MODO_PICOS) {
            if (pico[c] > r) {
                if (saida_pico[c]) {
                    saida_pico[c]--;
                    vai = r;
                    r = pico[c];
                }//if
                else {
                    pico[c]--;
                    saida_pico[c] = 12;
                }//else
            }//if
            else {
                pico[c] = r;
                saida_pico[c] = 12;
            }//else
        }//if

        uint8_t pontos = 12;
        if (modo & MODO_PONTOS) pontos = 1;
        if (modo & MODO_2PONTOS) pontos = 2;

        while (r) {
            r--;
            uint8_t r_val = lin_col[r%6] >> 4;
            uint8_t r_alt = (r/6) * 16;
            if (r_val&0x08)
                *(pout+r_alt+1) |= 1<<(r_val&0x07);
            else
                *(pout+r_alt) |= 1<<r_val;
            if (vai) {
                r = vai;
                vai = 0;
            }//if
            else {
                pontos--;
                if (!pontos) break;
            }//else
        }//while
    }//for
}

//________________________________________________________________________________
void scan() {

    uint8_t *pout = saida + (col<<1);
    uint8_t p1=*pout, p2=*(pout+1);

    col++;
    col &= 0x0f;

    P1DIR = P2DIR = 0;
    P1OUT = P2OUT = 0;
    if (!col) return;
    P1DIR = p1|EQ_RESET_PIN|EQ_STROBE_PIN;
    P2DIR = p2;
    p1 &= p1Lin;
    p2 &= p2Lin;
    if (!(col&0x08)) {
        p1 ^= 0xf8;
        p2 ^= 0xff;
    }//if
    p1 &= ~0x07;
    P1OUT = p1;
    P2OUT = p2;

}

//________________________________________________________________________________
int main(void) {
    WDTCTL = WDTPW + WDTHOLD;
    BCSCTL1 = CALBC1_8MHZ;
    DCOCTL  = CALDCO_8MHZ;

    P2SEL = P1SEL = 0;
    P1DIR = 0x00; P1OUT = 0x00;

    P1DIR = EQ_RESET_PIN|EQ_STROBE_PIN;
    TA0CCR0 = 576;                  // 576/72us
    TA0CTL = TASSEL_2 + MC_1;// SMCLK,

    ADC10CTL1 = INCH_0;
    ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE;

    _BIS_SR(GIE);

    uint8_t ocnt=0;

    modo |= MODO_2PONTOS;
    modo = MODO_PONTOS;
    //modo |= MODO_PICOS;

    load();
    while (1) {
        P1DIR = EQ_RESET_PIN|EQ_STROBE_PIN;
        P1OUT = 0x00;

        const uint8_t fator = 2;
        uint8_t i=0, j=0;
        for (sAtual=0;sAtual<fator;sAtual++) {
            TA0CCTL0 = CCIE;
            __bis_SR_register(LPM0_bits);
            TA0CCTL0 = 0;
        }//for
        for (i=0;i<7;i++) {
            uint8_t usa = 0, bom = 1;
            for (j=0;j<fator;j++) {
                if (amostra[j][i])
                    usa += amostra[j][i];
                else
                    bom = 0;
            }//if
            if (!bom) usa = 0;
            if (usa) {
                usa /= fator;
            }//use
            if (nivel[i]) nivel[i]--;
            if (usa > nivel[i]) nivel[i] = usa;
            if (modo & MODO_PICOS) nivel[i] = usa;
        }//for

        load();

        //___________ delay de espera
        uint16_t cnt = 200 * 16;
        while (cnt--) scan();

        ocnt++;
    }//while



}

uint8_t contador = 0;
uint8_t coluna = 0;
uint8_t txDado = 0;
uint8_t txconta = 0;
uint8_t eqConta = 0;
uint8_t eqCtrl = 0;

uint8_t cnt2=0;
//______________________________________________________________________
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0_iSR(void) {

    eqCtrl = (eqConta >> 2) & 0x07;
    if (eqCtrl == 0) {
        if (eqConta == 0) {
            P1OUT |= EQ_RESET_PIN;
            P1OUT &= ~EQ_RESET_PIN;
            eqConta += 4;
        }//if
    } else if ((eqConta & 0x03) == 3) {
        ADC10CTL0 |= ENC + ADC10SC;
    } else if (eqConta & 0x01) {
        P1OUT |= EQ_STROBE_PIN;
    } else if (eqConta & 0x02) {
        P1OUT &= ~EQ_STROBE_PIN;
    }//if
    eqConta++;
    contador++;

  //  timer_a();

    if (!(contador&0x1f))
        __bic_SR_register_on_exit(LPM0_bits);
}

//______________________________________________________________________
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
    //_______ 7 bandas.. 63, 160, 400, 1000, 2500, 6250, 16000
    //_______ compensaÃ§Ã£o para ruÃ­do
    const uint8_t  comp[] = { 25, 30, 25, 35, 30, 50, 65, };    // id 0542
    //const uint8_t  comp[] = { 95, 85, 60, 70, 85, 90, 90, };
    //const uint8_t  comp[] = { 40, 40, 40, 40, 40, 40, 40, };
    //_______ mapeamento logarÃ­tmico dos valores Analogicos para os nÃ­veis, ajuste para sua entrada
    const uint16_t lvls[] = { 0,
      4, 6, 8, 11, 16, 22, 32, 45, 63, 89, 126, 178, };         // 256
     //16, 22, 32, 45, 63, 90, 126, 179, 252, 356, 504, 711, }; // 1024
     // 8, 11, 16, 22, 32, 45, 63, 89, 126, 178, 252, 356, };   // 512
     // 6, 8, 12, 17, 24, 34, 47, 67, 95, 134, 189, 267, };     // 384


    // dimenciona de 1024 atÃ© 12

    uint16_t v = ADC10MEM;
    uint8_t i=12, c=eqCtrl-1;
    uint16_t amb = comp[c];

    if (v > amb)
        v -= amb;
    else
        v = 0;

    v *= 2;

    do {
        if (v >= lvls[i]) break;
    } while (i--);

    amostra[sAtual][c] = i;

}
