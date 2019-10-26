#include "LedControl.h"
#include "arduinoFFT.h"

const uint16_t t_amostra = 16; // 16-32-64-128
uint16_t k = 0;
double valorReal[t_amostra];
double valorImag[t_amostra];

arduinoFFT FFT = arduinoFFT();
LedControl lc = LedControl(12,11,10,1);

     /* Pinos da matriz 8x8
      CLK pin 11;  CS  pin 10;  DIN pin 12;

      Pinos do modulo de som
      A0 pin A0;
      */

      
void setup(){
   lc.shutdown(0,false);
   lc.setIntensity(0,8);  //Intensidade do LED
   lc.clearDisplay(0);

   writeIntro();
}

void loop(){
   for(int i=0; i<t_amostra; i++){
      valorReal[i] = analogRead(A0);
      valorImag[i] = 0;
   }
   
   FFT.Windowing(valorReal, t_amostra, FFT_WIN_TYP_RECTANGLE, FFT_FORWARD);
   FFT.Compute(valorReal, valorImag, t_amostra, FFT_FORWARD);
   FFT.ComplexToMagnitude(valorReal, valorImag, t_amostra);

   k = (t_amostra/2)-1;

   for(uint16_t i = 0; i < (k+1); i++){
      if((byte)valorReal[i]<10)
         lc.setRow(0,k,1);
      else 
         if((byte)valorReal[i]<30)
            lc.setRow(0,k,3);//3
         else 
            if((byte)valorReal[i]<90)
               lc.setRow(0,k,7);//7
            else 
               if((byte)valorReal[i]<110)
                  lc.setRow(0,k,15);//15
               else 
                  if((byte)valorReal[i]<130)
                     lc.setRow(0,k,31);//31
                  else 
                     if((byte)valorReal[i]<150)
                        lc.setRow(0,k,63);//63
                     else 
                        if((byte)valorReal[i]<170)
                           lc.setRow(0,k,127);//127
                        else 
                           if((byte)valorReal[i]<190)
                              lc.setRow(0,k,255);   //255
       if(k>0)
          k--;
   // delay(5);
   }
}

void writeIntro() {
  
  byte U[6]={B11111100,B00000010,B00000010,B00000010,B11111100,B00000000};
  byte n[6]={B00111110,B00010000,B00100000,B00100000,B00011110,B00000000};
  byte B[6]={B11111110,B10010010,B10010010,B10010010,B01101100,B00000000};
  
  lc.setRow(0,0,U[5]);
  lc.setRow(0,1,U[4]);
  lc.setRow(0,2,U[3]);
  lc.setRow(0,3,U[2]);
  lc.setRow(0,4,U[1]);
  lc.setRow(0,5,U[0]);
  lc.setRow(0,6,U[5]);
  lc.setRow(0,7,U[5]);
  delay(800);
  
  lc.setRow(0,0,n[5]);
  lc.setRow(0,1,n[4]);
  lc.setRow(0,2,n[3]);
  lc.setRow(0,3,n[2]);
  lc.setRow(0,4,n[1]);
  lc.setRow(0,5,n[0]);
  lc.setRow(0,6,n[5]);
  lc.setRow(0,7,n[5]);
  delay(800);
  
  lc.setRow(0,0,B[5]);
  lc.setRow(0,1,B[4]);
  lc.setRow(0,2,B[3]);
  lc.setRow(0,3,B[2]);
  lc.setRow(0,4,B[1]);
  lc.setRow(0,5,B[0]);
  lc.setRow(0,6,B[5]);
  lc.setRow(0,7,B[5]);
  delay(800);
  

}
