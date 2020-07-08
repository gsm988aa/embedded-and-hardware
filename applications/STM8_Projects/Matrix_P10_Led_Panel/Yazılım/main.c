#include <18F4620.h>

#device adc=10

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES WDT128                   //Watch Dog Timer uses 1:128 Postscale
#FUSES H4                       //High speed osc with HW enabled 4X PLL
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOXINST                  //Extended set extension and Indexed Addressing mode disabled (Legacy mode)

#use delay(clock=40000000)

#use fast_io(a)
#use fast_io(b)
#use fast_io(c)
#use fast_io(d)
#use fast_io(e)

#include <math.h>
/*******************************P10 Panel kütüphanesi**************************/
#include <P10_Panel_Driver.c>
/********************************Global Deðiþkenler****************************/
unsigned int8 int_syc=0;
int i=0,w=0;
/**********************************Fonksiyonlar********************************/
void write_data    (unsigned int8 row_adr);

void FontTest      (void);
void NumberTest    (void);
void ScrollTest    (void);
void BrightTest    (void);
void GeoShapes     (void);
void SineWawe      (void);
void BarTest       (void);
/********************************Kesme Alt Programý****************************/
#int_timer0                      //Timer0 Kesmesi
void Timer0_int(){
set_timer0(133);                 //800us Gecikme için Timer kuruluyor..
switch (int_syc)   //Her 800us de Bir bir sonraki satýra Geç
{
case 0:
     output_low(PinOE);write_data(int_syc);
     output_low(PinA);output_low(PinB);output_high(PinOE);
     break;
case 1:
     output_low(PinOE);write_data(int_syc);
     output_high(PinA);output_low(PinB);output_high(PinOE);
     break;
case 2:
     output_low(PinOE);write_data(int_syc);
     output_low(PinA);output_high(PinB);output_high(PinOE);
     break;
case 3:
     output_low(PinOE);write_data(int_syc);
     output_high(PinA);output_high(PinB);output_high(PinOE);
     break;
}
int_syc++;if(int_syc>3)int_syc=0;
}

/**********************************Ana Program*********************************/
void main()
{
   set_tris_a(0x03);
   set_tris_b(0x00);
   set_tris_c(0x00);
   set_tris_d(0x00);
   set_tris_e(0x00);
   
   setup_spi(SPI_MASTER | SPI_L_TO_H | SPI_SS_DISABLED);
   setup_adc(ADC_OFF);
   setup_ccp1(CCP_PWM);
   setup_ccp2(CCP_OFF);
   setup_timer_0(T0_INTERNAl | T0_8_BIT | T0_DIV_64);
   setup_timer_2(T2_DIV_BY_4,249,1);
   enable_interrupts(INT_timer0);
   enable_interrupts(GLOBAL);
   set_timer0(133);
   
   output_a(0x00);
   output_b(0x00);
   output_c(0x00);
   output_d(0x00);
   output_e(0x00);
   
   Set_Brightness(10);   //Parlaklýk veriliyor.
   clear_display(0);     //Ekraný Temizle
   
      Text="P10";
      ShowString(7,0,1,0);
      Text="Test";
      ShowString(4,8,1,0);
      delay_ms(1500);
      for (i=0;i<32;i++)
      {
        for (w=0;w<16;w++)
        {
           LineScroll(w,0);
        }
        delay_ms(30);
      }
  
   while(TRUE)
   {
      FontTest();
      delay_ms(1000);
      NumberTest();
      delay_ms(1000);
      ScrollTest();
      delay_ms(1000);
      BrightTest();
      delay_ms(1000);
      GeoShapes();
      delay_ms(1000);
      SineWawe();
      delay_ms(1000);
      BarTest();
      delay_ms(2000);
      
      Text="www.arectron.com      ";
      TextToLeftScroll(4,1,9,0);
      
      delay_ms(2000);
   }
}

void write_data(unsigned int8 row_adr)
{
   int8 i=0,j=0;
   int8 k=3;
   while(k!=255)                                   // Bu Döngü ileriye doðru sayarsa panel saðdan 
   {j=15-k;                                        // Sola Doðru yüklenir. 
     for (i=0;i<4;i++)                             // Satýrlar dolduruluyor..
     {
       spi_write(display_ram[j][row_adr]);
       j-=4;
     }
     k--;
   }
   output_high(LATCH);
   delay_cycles(2);
   output_low(LATCH);
}

void FontTest(void){
   Text="Font";
   ShowString(3,0,1,0);
   Text="Test";
   ShowString(3,8,1,0);
   delay_ms(1500);
   clear_display(0);
   delay_ms(100);
   
   
   Text="A";
   for (i=1;i<5;i++)
   {
     ShowString(0,0,i,0);
     delay_ms(400);
     clear_display(0);
   }
   Text="B";
   for (i=1;i<5;i++)
   {
     ShowString(0,0,i,0);
     delay_ms(400);
     clear_display(0);
   }
   Text="C";
   for (i=1;i<5;i++)
   {
     ShowString(0,0,i,0);
     delay_ms(400);
     clear_display(0);
   }
   Text="D";
   for (i=1;i<5;i++)
   {
     ShowString(0,0,i,0);
     delay_ms(400);
     clear_display(0);
   }
}

void NumberTest(void){
   Text="Numbr";
   ShowString(0,0,1,0);
   Text="Test";
   ShowString(0,8,1,0);
   delay_ms(1000);
   clear_display(0);
   delay_ms(100);
   for (i=0;i<100;i++)
   {
      sprintf(Text,"%02d",i);
      ShowString(4,0,4,0);
      delay_ms(100);
   }
   for (i=0;i<3;i++)
   {
     Text="--";//
     ShowString(4,0,4,0);
     delay_ms(700);
     clear_display(0);
     delay_ms(500);
   } 
   clear_display(0);
   Text="iNVRT";
   ShowString(0,4,1,0);
   delay_ms(1000);
   clear_display(1);
   delay_ms(100);
   for (i=0;i<100;i++)
   {
      sprintf(Text,"%02d",i);
      ShowString(4,0,4,1);
      delay_ms(100);
   }
   for (i=0;i<3;i++)
   {
     Text="--";//
     ShowString(4,0,4,1);
     delay_ms(700);
     clear_display(1);
     delay_ms(500);
   } 
   delay_ms(500);
      for (i=0;i<32;i++)
      {
        for (w=0;w<16;w++)
        {
           LineScroll(w,0);
        }
        delay_ms(15);
      }   
   
}

void ScrollTest(void){
   Text="Scrol";
   ShowString(0,0,1,0);
   Text="Test";
   ShowString(0,8,1,0);
   delay_ms(1000);
   clear_display(0);
   delay_ms(100);
   
     Text="FONT 1     ";
     TextToLeftScroll(0,1,9,0);
     delay_ms(800);
     Text="FONT 2     ";
     TextToLeftScroll(0,2,9,0);
     delay_ms(700);
     Text="FONT 3     ";
     TextToLeftScroll(0,3,9,0);
     delay_ms(600);
     Text="FONT 4     ";
     TextToLeftScroll(0,4,9,0);
     delay_ms(500);
     clear_display(1);
     Text="Hardware  Pic18F4620 & 40Mhz, 64Kb Flash, 3968Byte Ram vs..    ";
     TextToLeftScroll(0,4,9,1);    
     clear_display(0);
}

void BrightTest(void)
{
   for(i=1;i<100;i++)
   {  
       Text="Brigt";
       ShowString(0,0,1,0);
       sprintf(Text,"%02d",i);
       ShowString(10,8,1,0);
       Set_Brightness(i);
       delay_ms(150);
   }
  Set_Brightness(100);
  clear_display(0);
  Text="MAX";
  for(i=0;i<4;i++)
  {
    ShowString(4,3,2,0);
    delay_ms(800);
    clear_display(0);
    delay_ms(500);
  }
  Set_Brightness(10);

}

void GeoShapes(void){
  Rectangle(14,6,17,9,0);
  delay_ms(600);
  clear_display(0);
  Rectangle(11,3,20,12,0);
  delay_ms(600);
  clear_display(0);
  Rectangle(8,0,23,15,0);
  delay_ms(600);
  clear_display(0);
  Rectangle(14,6,17,9,1);
  delay_ms(600);
  clear_display(0);
  Rectangle(11,3,20,12,1);
  delay_ms(600);
  clear_display(0);
  Rectangle(8,0,23,15,1);
  delay_ms(1000);
  clear_display(0);
  Circle(16,8,2,0);
  delay_ms(600);
  clear_display(0);
  Circle(16,8,4,0);
  delay_ms(600);
  clear_display(0);
  Circle(16,8,7,0);
  delay_ms(600);
  clear_display(0);
  Circle(16,8,2,1);
  delay_ms(600);
  clear_display(0);
  Circle(16,8,4,1);
  delay_ms(600);
  clear_display(0);
  Circle(16,8,7,1);
  delay_ms(1000);
  clear_display(0);
  delay_ms(100);
  Line(0,0,31,15,1);
  delay_ms(600);
  Line(0,15,31,0,1);
  delay_ms(600);
  Line(5,0,8,15,1);  
  delay_ms(600);
  Line(31,8,0,4,1);
  delay_ms(600);
  Line(29,0,10,15,1);
  delay_ms(600);
  Line(31,13,0,8,1);
  delay_ms(1000);
  
  clear_display(0);  
}

void SineWawe(void){
  float ax=0;
  int16 xpos=0;
  int8  ypos=0,sayac=0;
  Text="Sine";
  ShowString(0,0,1,0);
  Text="Wave";
  ShowString(0,8,1,0);
  delay_ms(1000);
  clear_display(0);
  delay_ms(100);
  while(True)
  {
  for(xpos=0;xpos<19;xpos++)
  {
    for (i=0;i<16;i++){ 
      LineScroll(i,0);
    }
    ax=(pi*xpos)/180;
    ypos=8+(7*sin(19*ax));  
    PutPixel(31,ypos,1);
    delay_ms(30);
  }
  sayac++;if(sayac==10)break;
 }  
 clear_display(0);
}

void BarTest(void){
int xstart=0,xend=3;

Text="Bar";
ShowString(0,0,1,0);
Text="Test";
ShowString(0,8,1,0);
delay_ms(1000);
clear_display(0);
delay_ms(100);

for(w=0;w<8;w++)
{
  for(i=0;i<101;i++)
  {
    Bargraph(xstart,0,xend-1,15,1,i);
    delay_ms(2);
  }
  xstart=xstart+4;xend=xend+4;
}
xstart=0;xend=3;
for(w=0;w<8;w++)
{
  for(i=0;i<101;i++)
  {
    Bargraph(xstart,0,xend-1,15,1,100-i);
    delay_ms(2);
  }
  xstart=xstart+4;xend=xend+4;
}
clear_display(0);
xstart=0;xend=3;
for (w=0;w<4;w++)
{
  for (i=0;i<101;i++)
  {
    Bargraph(0,xstart,31,xend-1,0,i);
    delay_ms(2);
  }
  xstart=xstart+4;xend=xend+4;
}
xstart=0;xend=3;
for (w=0;w<4;w++)
{
  for (i=0;i<101;i++)
  {
    Bargraph(0,xstart,31,xend-1,0,100-i);
    delay_ms(2);
  }
  xstart=xstart+4;xend=xend+4;
}
clear_display(0);
for(w=0;w<2;w++)
{
  for(i=0;i<101;i++)
  {
    Bargraph(0,0,31,15,1,i);
    delay_ms(1);
  }
  for(i=0;i<101;i++)
  {
    Bargraph(0,0,31,15,1,100-i);
    delay_ms(1);
  }
}
clear_display(0);
for(w=0;w<2;w++)
{
  for(i=0;i<101;i++)
  {
    Bargraph(0,0,31,15,0,i);
    delay_ms(1);
  }
  for(i=0;i<101;i++)
  {
    Bargraph(0,0,31,15,0,100-i);
    delay_ms(1);
  }
 clear_display(0);
}

}
