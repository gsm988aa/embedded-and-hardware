/* This example is to illustrate the use of the functon <setbrightness(uint8_t brightness)>
 * this function takes one argument of type uint8_t, it's value can be from 0 to 255, 255 being highest brightness and 0 is lowest.
 * this function can be called anytime after or before calling any other function of LedP10 library.
 * In this example brightness reduces when counter <num1> reaches value of 50.
 * For connection diagram visit https://goo.gl/HiCJC1  
 */

#include "TimerOne.h"
#include "SPI.h"
#include "ledP10.h"
  
int num1=0;
LedP10 myled;

void setup() 
{
  myled.init(3,4,8,9,1);
}

void loop() {
  if(num1==50)
  {
    myled.setbrightness(50);
  }
  myled.showmsg_single_static(num1,0);
  num1+=1;  
  delay(500);
}
