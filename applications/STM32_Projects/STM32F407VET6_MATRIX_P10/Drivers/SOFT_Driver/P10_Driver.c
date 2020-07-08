/***************************************************************************************
***************************************************************************************/
#include <font1.h>
#include <font2.h>
#include <font3.h>
#include <font4.h>
/***************************************************************************************
***************************************************************************************/
#include "P10_Driver.h"
/***************************************************************************************
***************************************************************************************/
extern uint8_t Text[100];
uint8_t display_ram[16][4];
uint8_t imageBuffer[64];
uint8_t int_syc = 0;
/***************************************************************************************
***************************************************************************************/
void HC595_Write(uint8_t data)
{
    for(uint8_t i=0; i<8; i++)
    {
        if(((data<<i)&0x80)==0x80)GPIO_P10_PIN_DATA_HIG;
        else GPIO_P10_PIN_DATA_LOW;
        GPIO_P10_PIN_CLOCK_HIG;
        GPIO_P10_PIN_CLOCK_LOW;
    }
}
/***************************************************************************************
***************************************************************************************/
void write_data(uint8_t row_adr)
{
    uint8_t i=0, j=0;
    uint8_t k=3;
    while(k!=255)
    {
        j=15-k;
        for(i=0; i<4; i++)
        {
            HC595_Write(display_ram[j][row_adr]);
            j-=4;
        }
        k--;
    }
    GPIO_P10_PIN_LATCH_HIG;
    GPIO_P10_PIN_LATCH_LOW;
}
/***************************************************************************************
***************************************************************************************/
void ScanMatrix(void)
{
    switch (int_syc)
    {
        case 0:
            GPIO_P10_PIN_OE_LOW;
            write_data(int_syc);
            GPIO_P10_PIN_A_LOW;
            GPIO_P10_PIN_B_LOW;
            GPIO_P10_PIN_OE_HIG;
            break;
        case 1:
            GPIO_P10_PIN_OE_LOW;
            write_data(int_syc);
            GPIO_P10_PIN_A_HIG;
            GPIO_P10_PIN_B_LOW;
            GPIO_P10_PIN_OE_HIG;
            break;
        case 2:
            GPIO_P10_PIN_OE_LOW;
            write_data(int_syc);
            GPIO_P10_PIN_A_LOW;
            GPIO_P10_PIN_B_HIG;
            GPIO_P10_PIN_OE_HIG;
            break;
        case 3:
            GPIO_P10_PIN_OE_LOW;
            write_data(int_syc);
            GPIO_P10_PIN_A_HIG;
            GPIO_P10_PIN_B_HIG;
            GPIO_P10_PIN_OE_HIG;
            break;
    }
    int_syc++;if(int_syc>3)int_syc=0;
}
/***************************************************************************************
***************************************************************************************/
int CharAdres(char c)
{
    int adr=0;
    switch(c)
    {

    }
    return adr;
}
/***************************************************************************************
***************************************************************************************/
uint8_t LineScroll(uint8_t Line, uint8_t zerobit)
{
    uint32_t Tam=0;
    uint8_t y1=Line%4;
    uint8_t y2=Line/4;
    uint8_t OwerFlow;
    zerobit=1-zerobit;
  
    OwerFlow=(display_ram[(y2*4)][y1]/128)&0x01;
    Tam=display_ram[(y2*4)][y1];
    Tam=Tam<<8;
    Tam=Tam+display_ram[(y2*4)+1][y1];
    Tam=Tam<<8;
    Tam=Tam+display_ram[(y2*4)+2][y1];
    Tam=Tam<<8;
    Tam=Tam+display_ram[(y2*4)+3][y1];
  
    Tam=Tam<<1;
    Tam=Tam+zerobit;
  
    display_ram[(y2*4)  ][y1]=(uint32_t)(Tam>>24)&0xFF;
    display_ram[(y2*4)+1][y1]=(uint32_t)(Tam>>16)&0xFF;
    display_ram[(y2*4)+2][y1]=(uint32_t)(Tam>>8)&0xFF;
    display_ram[(y2*4)+3][y1]=(uint32_t)(Tam&0xFF);
    return OwerFlow;
}
/***************************************************************************************
***************************************************************************************/
void DrawCharFont1(uint8_t x, uint8_t y, uint8_t fill)
{
    uint8_t a=0, b=0, i=0;
    uint8_t cdata=0;
    uint8_t clm=x, row=y; 
    while (Text[i]!='\0')
    {
        if(clm+5>WIDTH)
        { 
            clm=0;         
            row = row+8;
            if(row+8>HEIGHT+1)break;
        }
        for(b=0; b<5; b++)
        {
            cdata=Font_5x7[Text[i]-32][b];
            for(a=0; a<8; a++)
            {
                PutPixel(clm+b, row+a, (fill-((cdata>>a) & 0x01)));
            }
        }
        i++;
        clm=clm+6;
    }  
}
/***************************************************************************************
***************************************************************************************/
void DrawCharFont2(uint8_t x, uint8_t y, uint8_t fill)
{
    uint8_t a=0,b=0,i=0;
    uint8_t cdata=0;
    uint8_t clm=x,row=y;
    while(Text[i]!='\0')
    {  
        if(clm+8>WIDTH)
        { 
            clm=0;         
            row=row+12;
            if(row+12>HEIGHT+1) break;
        }
      
       for(b=0; b<16; b=b+2)
       {
            cdata=Font_8x12[Text[i]-32][b];
            for(a=0; a<8; a++)
            {
                PutPixel(clm+(b/2),row+a,(fill-((cdata>>a) & 0x01)));
            }
            cdata=Font_8x12[Text[i]-32][b+1];
            for(a=0; a<4; a++)
            {
                PutPixel(clm+(b/2), row+8+a, (fill-((cdata>>a) & 0x01)));
            }
        }
        clm=clm+8;
        i++;
    }
}
/***************************************************************************************
***************************************************************************************/
void DrawCharFont3(uint8_t x, uint8_t y, uint8_t fill)
{
    uint8_t a=0,b=0,i=0;
    uint8_t cdata=0;
    uint8_t clm=x,row=y;
    while(Text[i]!='\0')
    {  
        if(clm+12>WIDTH)  //Satýr sonuna gelindimi Bir Alt Satýra Geç
        { 
            clm=0;         
            row=row+14;
            if(row+14>HEIGHT+1) break;
        }
        for(b=0; b<24; b=b+2)
        {
            cdata=Font_12x14[Text[i]-32][b];
            for(a=0;a<8;a++)
            {
                PutPixel(clm+(b/2), row+a, (fill-((cdata>>a) & 0x01)));
            }
            cdata=Font_12x14[Text[i]-32][b+1];
            for(a=0;a<6;a++)
            {
                PutPixel(clm+(b/2), row+8+a, (fill-((cdata>>a) & 0x01)));
            }
        }
        clm=clm+12;
        i++;
    }
}
/***************************************************************************************
***************************************************************************************/
void DrawCharFont4(uint8_t x, uint8_t y, uint8_t fill)
{
    uint8_t a=0,b=0,i=0;
    uint8_t cdata=0;
    uint8_t cadr=0;
    uint8_t clm=x,row=y;

    while(Text[i]!='\0')
    {  
        if(clm+12>WIDTH)  //Satýr sonuna gelindimi Bir Alt Satýra Geç
        { 
            clm=0;         
            row=row+16;
            if(row+16>HEIGHT+1) break;
        }
        cadr=CharAdres(Text[i]);
        for(b=0; b<24; b=b+2)
        {
            cdata=Font_12x16[cadr][b];
            for(a=0; a<8; a++)
            {
                PutPixel(clm+(b/2), row+a, (fill-((cdata>>a) & 0x01)));
            }
            cdata=Font_12x16[cadr][b+1];
            for(a=0; a<8; a++)
            {
                PutPixel(clm+(b/2), row+8+a, (fill-((cdata>>a) & 0x01)));
            }
        }
        clm=clm+12;
        i++;
    }
}
/***************************************************************************************
***************************************************************************************/
void ScrollTextFont1(uint8_t Line, uint8_t speed, uint8_t fill)
{
    uint8_t a=0,b=0,i=0;
    uint8_t cdata=0;
    uint8_t zerobit=0; 
    
    while(Text[i]!='\0')
    {
        if(Line+Font1H-1>HEIGHT)break;
        for(b=0; b<5; b++)
        {
            cdata=Font_5x7[Text[i]-32][b];
            for (a=0; a<8; a++)
            {
                zerobit=fill-((cdata>>a) & 0x01);
                LineScroll(Line+a, zerobit);
            }
            delay_10us(speed);
        }
        for (a=0; a<8; a++)
        {
            LineScroll(Line+a,0);
        }
        delay_10us(speed);
        i++;
    }
}
/***************************************************************************************
***************************************************************************************/
void ScrollTextFont2(uint8_t Line, uint8_t speed, uint8_t fill)
{
    uint8_t a=0,b=0,i=0;
    uint8_t cdata=0;
    uint8_t zerobit=0; 
  
    while(Text[i]!='\0')
    { 
        if(Line+Font2H-1>HEIGHT)break;
        for(b=0; b<16; b=b+2)
        {
            cdata=Font_8x12[Text[i]-32][b];
            for(a=0; a<8; a++)
            {
                zerobit=fill-((cdata>>a) & 0x01);
                LineScroll(Line+a, zerobit);
            }  
            cdata=Font_8x12[Text[i]-32][b+1];
            for(a=0; a<4; a++)
            {
                zerobit=fill-((cdata>>a) & 0x01);
                LineScroll(Line+8+a, zerobit);
            } 
            delay_10us(speed);
        }
        i++;
    }
}
/***************************************************************************************
***************************************************************************************/
void ScrollTextFont3(unsigned int Line, unsigned int speed, uint8_t fill)
{
    uint8_t a=0,b=0,i=0;
    uint8_t cdata=0;
    uint16_t delay=speed*20;
    uint16_t sayac=0;
    uint8_t zerobit=0; 
 
    while(Text[i]!='\0')
    {
        if(Line+Font3H-1>HEIGHT)break;
        for(b=0;b<24;b=b+2)
        {
            cdata=Font_12x14[Text[i]-32][b];
            for(a=0; a<8; a++)
            {
                zerobit=fill-((cdata>>a) & 0x01);
                LineScroll(Line+a, zerobit);
            }  
            cdata=Font_12x14[Text[i]-32][b+1];
            for(a=0; a<6; a++)
            {
                zerobit=fill-((cdata>>a) & 0x01);
                LineScroll(Line+8+a,zerobit);
            } 
            for(sayac=0; sayac<210-delay; sayac++)
            {
                delay_10us(1);
            }
        }
        i++;
    }
}
/***************************************************************************************
***************************************************************************************/
void ScrollTextFont4(unsigned int Line, unsigned int speed, uint8_t fill)
{
    uint8_t a=0,b=0,i=0;
    uint8_t cdata=0,cadr=0;;
    uint32_t delay=speed*20;
    uint32_t sayac=0;
    uint8_t zerobit=0; 
  
    while(Text[i]!='\0')
    {
        if(Line+Font4H-1>HEIGHT)break;
        cadr=CharAdres(Text[i]);
    
        for(b=0; b<24; b=b+2)
        {
            cdata=Font_12x16[cadr][b];
            for(a=0; a<8; a++)
            {
                zerobit=fill-((cdata>>a) & 0x01);
                LineScroll(a, zerobit);
            }    
            cdata=Font_12x16[cadr][b+1];
            for(a=0; a<8; a++)
            {
                zerobit=fill-((cdata>>a) & 0x01);
                LineScroll(8+a, zerobit);
            } 
            for(sayac=0; sayac<210-delay; sayac++)
            {
                delay_10us(1);
            }
        }
        i++;
    }
}
/***************************************************************************************
***************************************************************************************/
void clear_display(uint8_t fill)
{
    uint8_t i=0, j=0, fdata=0;
    if(fill)fdata=0x00;
    else fdata=0xff;
    for(i=0; i<4; i++)
    {
        for(j=0; j<16; j++)
        {
            display_ram[j][i]=fdata;
        }
    }
}
/***************************************************************************************
***************************************************************************************/
void invert_screen(void)
{
    uint8_t i=0,j=0;
 
    for(i=0; i<4; i++)
    {
        for(j=0; j<16; j++)
        {
            display_ram[j][i] =~ display_ram[j][i];  
        }
    }
}
/***************************************************************************************
***************************************************************************************/
void Set_Brightness(uint8_t value)
{
    //set_pwm1_duty((uint16_t)value*10);
}
/***************************************************************************************
***************************************************************************************/
void PutPixel(uint8_t Xpos, uint8_t Ypos, uint8_t fill)
{
    uint8_t y1=0,x1=0;
    uint8_t y2=0,x2=0;
    uint8_t temp=0;
            
    y1 = Ypos%4;
    y2 = Ypos/4;
    x1 = Xpos%8;    
    x2 = Xpos/8;
  
    if(fill)
    {
        temp=display_ram[(y2*4)+x2][y1];
        display_ram[(y2*4)+x2][y1] = (temp & (0xff-(0x80>>x1)));
    }
    else
    {
        temp=display_ram[(y2*4)+x2][y1];
        display_ram[(y2*4)+x2][y1] = (temp | (0x80>>x1));
    }
 
}
/***************************************************************************************
***************************************************************************************/
uint8_t GetPixel(uint8_t Xpos, uint8_t Ypos)
{
    uint8_t y1=0,x1=0;
    uint8_t y2=0,x2=0;
    uint8_t temp=0;
    uint8_t value=0;
    
    y1 = Ypos%4;
    y2 = Ypos/4;
    x1 = Xpos%8;    
    x2 = Xpos/8;
    
    temp=display_ram[(y2*4)+x2][y1];
    value=1-(0x80 & temp<<x1)/128;
    return value;
}
/***************************************************************************************
***************************************************************************************/
void Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t fill)
{
    signed int addx=1, addy=1, P;
    signed int i, dy, dx, diff;
  
    if(x2>x1) 
    {
        dx = x2 - x1;
    }
    else
    {
        dx = x1 - x2;
        addx = -1;
    }
    if(y2>y1)
    {
        dy = y2 - y1;
    }
    else
    {
        dy = y1 - y2;
        addy = -1;
    }
    
    if(dx >= dy)
    {
        dy *= 2;
        P = dy - dx;
        diff = P - dx;
    
        for(i=0; i<=dx; i++)
        {
            PutPixel(x1, y1, fill);
    
            if(P < 0)
            {
                P  += dy;
                x1 += addx;
            }
            else
            {
                P  += diff;
                x1 += addx;
                y1 += addy;
            }
        }
    }
    else
    {
        dx *= 2;
        P = dx - dy;
        diff = P - dy;
    
        for(i=0; i<=dy; ++i)
        {
            PutPixel(x1, y1, fill);
    
            if(P < 0)
            {
                P  += dx;
                y1 += addy;
            }
            else
            {
                P  += diff;
                x1 += addx;
                y1 += addy;
            }
        }
    }   
}
/***************************************************************************************
***************************************************************************************/
void Rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t fill)
{
    uint16_t i, xmin, xmax, ymin, ymax;

    if(fill)
    {
        if(x1 < x2)
        {
            xmin = x1;
            xmax = x2;
        }
        else
        {
            xmin = x2;
            xmax = x1;
        }
    
        if(y1 < y2)
        {
            ymin = y1;
            ymax = y2;
        }
        else
        {
            ymin = y2;
            ymax = y1;
        }
    
        for(; xmin <= xmax; ++xmin)
        {
            for(i=ymin; i<=ymax; ++i)
            {
                PutPixel(xmin, i, 1);
            }
        }
    }
    else
    {
        Line(x1, y1, x2, y1, 1);
        Line(x2, y1, x2, y2, 1);
        Line(x2, y2, x1, y2, 1);
        Line(x1, y2, x1, y1, 1);
    }      
}
/***************************************************************************************
***************************************************************************************/
void Circle(uint8_t x, uint8_t y, uint8_t size, uint8_t fill)
{
    signed a, b, P;
    a=0;
    b=size;
    P=1-size;
    
    do
    {
        if(fill)
        {
            Line(x-a, y+b, x+a, y+b,1);
            Line(x-a, y-b, x+a, y-b,1);
            Line(x-b, y+a, x+b, y+a,1);
            Line(x-b, y-a, x+b, y-a,1);
        }
        else
        {
            PutPixel(a+x, b+y,1);
            PutPixel(b+x, a+y,1);
            PutPixel(x-a, b+y,1);
            PutPixel(x-b, a+y,1);
            PutPixel(b+x, y-a,1);
            PutPixel(a+x, y-b,1);
            PutPixel(x-a, y-b,1);
            PutPixel(x-b, y-a,1);
        }
        
        if(P < 0)
        P+= 3 + 2*a++;
        else
        P+= 5 + 2*(a++ - b--);
    }while(a<=b);

}
/***************************************************************************************
***************************************************************************************/
void Bargraph(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t hor, uint8_t value)
{
	uint8_t w=0;
	uint8_t h=0;
	uint8_t a=0,b=0;
    uint16_t barval=0;
    
    if(x1>x2) w=x1-x2;
    if(x1<x2) w=x2-x1;  
    if(y1>y2) h=y1-y2; 
    if(y1<y2) h=y2-y1;
    
    if(value>100)value=100;
    
    if(hor) //Bar Dikey Olarak Oluþturulacak
    {
        barval=((uint16_t)value*h)/100;
    
        for(a=0; a<h+1; a++)
        {
            for(b=0; b<w+1; b++)
            {
                if(barval>=a)
                {
                    PutPixel(x1+b,15-a+y1,1);
                }
                else
                {
                    PutPixel(x1+b,15-a+y1,0);
                }
            }
        }
        
    }
    else
    {  //Bar Yatay olarak Oluþturulacak    
        barval=((uint16_t)value*w)/100;
        
        for (a=0; a<w+1; a++)
        {
            for(b=0; b<h+1; b++)
            {
                if(barval>=a)
                {
                    PutPixel(x1+a,y1+b,1);
                }
                else
                {
                    PutPixel(x1+a,y1+b,0);
                }
            }
        }
    }
}
/***************************************************************************************
***************************************************************************************/
void ShowString(uint8_t x, uint8_t y, uint8_t size, uint8_t fill)
{
    switch(size)
    {
        case 1:  DrawCharFont1(x, y, fill);break;
        case 2:  DrawCharFont2(x, y, fill);break;
        case 3:  DrawCharFont3(x, y, fill);break;
        case 4:  DrawCharFont4(x, y, fill);break;
        default: break;
    }
}
/***************************************************************************************
***************************************************************************************/
void TextToLeftScroll(uint8_t Line, uint8_t size, uint8_t speed, uint8_t fill)
{
    switch(size)
    {
        case 1:  ScrollTextFont1(Line, speed, fill);break;
        case 2:  ScrollTextFont2(Line, speed, fill);break;
        case 3:  ScrollTextFont3(Line, speed, fill);break;
        case 4:  ScrollTextFont4(Line, speed, fill);break;
        default: break;
    }
}

void Writeimage()
{
    uint8_t a=0,b=0,i=0;
    uint8_t cdata=0;
  
    for(a=0; a<32; a++)
    {
        cdata=imageBuffer[i];
        for(b=0; b<8; b++)
        {
            PutPixel(a, b, (cdata>>b)&0x01); 
        }
        i++;
        cdata=imageBuffer[i];
        for(b=0;b<8;b++)
        {
            PutPixel(a,8+b,(cdata>>b)&0x01);
        }
        i++;
    }
}
